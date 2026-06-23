"""Python bindings for objective-abstract-quality."""

import ctypes
import os
import sys
from dataclasses import dataclass
from pathlib import Path

# ---- Locate and load the shared library --------------------------------

def _libname() -> str:
    if sys.platform.startswith("win"):
        return "oaq.dll"
    if sys.platform == "darwin":
        return "liboaq.dylib"
    return "liboaq.so"

_HERE = Path(__file__).resolve().parent
_LIB_PATH = _HERE.parent / _libname()      # adjust to wherever you build it
_lib = ctypes.CDLL(str(_LIB_PATH))

# ---- Mirror the C structs ----------------------------------------------

class _OaqAnalysis(ctypes.Structure):
    _fields_ = [
        ("acronym_score", ctypes.c_int),
        ("symbol_score",  ctypes.c_int),
        ("lexile_score",  ctypes.c_int),
        ("total_score",   ctypes.c_int),
        ("venue",         ctypes.c_char * 128),
    ]

class _OaqReview(ctypes.Structure):
    _fields_ = [
        ("name",       ctypes.c_char * 64),
        ("reader",     ctypes.c_int),
        ("accept_pct", ctypes.c_int),
        ("verdict",    ctypes.c_int),
        ("confidence", ctypes.c_int),
    ]

class _OaqSubmission(ctypes.Structure):
    _fields_ = [
        ("reviews",    _OaqReview * 4),
        ("n_reviews",  ctypes.c_int),
        ("n_accepts",  ctypes.c_int),
        ("decision",   ctypes.c_char * 128),
    ]

# ---- Bind function signatures ------------------------------------------

_lib.oaq_analyze.argtypes = [ctypes.c_char_p, ctypes.c_size_t, ctypes.POINTER(_OaqAnalysis)]
_lib.oaq_analyze.restype  = ctypes.c_int

_lib.oaq_submit.argtypes  = [ctypes.c_char_p, ctypes.c_size_t, ctypes.POINTER(_OaqSubmission)]
_lib.oaq_submit.restype   = ctypes.c_int

_lib.oaq_analyze_file.argtypes = [ctypes.c_char_p, ctypes.POINTER(_OaqAnalysis)]
_lib.oaq_analyze_file.restype  = ctypes.c_int

_lib.oaq_submit_file.argtypes  = [ctypes.c_char_p, ctypes.POINTER(_OaqSubmission)]
_lib.oaq_submit_file.restype   = ctypes.c_int

# ---- Public Python API --------------------------------------------------

@dataclass
class Analysis:
    acronym_score: int
    symbol_score:  int
    lexile_score:  int
    total_score:   int
    venue:         str

@dataclass
class Review:
    name:       str
    reader:     str   # "reviewer" | "student" | "nobody"
    accept_pct: int
    verdict:    str   # "accept" | "reject"
    confidence: int

@dataclass
class Submission:
    reviews:   list
    n_accepts: int
    decision:  str

_READER = {0: "reviewer", 1: "student", 2: "nobody"}
_VERDICT = {0: "accept", 1: "reject"}

def _as_bytes(abstract) -> bytes:
    if isinstance(abstract, bytes):
        return abstract
    if isinstance(abstract, str):
        return abstract.encode("utf-8")
    # Accept a path
    return Path(abstract).read_bytes()

def format_analysis(a: Analysis) -> str:
    lines = [
        "[analysis mode]",
        "spinning up advanced AI...",
        "",
        f"  acronym score:  {a.acronym_score}",
        f"  symbol score:   {a.symbol_score}",
        f"  lexile score:   {a.lexile_score}",
        "",
        "=========================================",
        f" objective abstract quality score: {a.total_score}",
        f" predicted venue: {a.venue}",
        "=========================================",
    ]
    return "\n".join(lines)


_READER_LABEL = {
    "reviewer": "reviewer (read it carefully)",
    "student":  "grad student (skimmed during lab meeting)",
    "nobody":   "nobody (verdict generated on the flight)",
}

def format_submission(s: Submission) -> str:
    lines = ["[submission mode]", "notifying four reviewers...", ""]
    for i, r in enumerate(s.reviews, start=1):
        lines += [
            f"---- Review {i} ----",
            f"  reviewer:    {r.name}",
            f"  read by:     {_READER_LABEL.get(r.reader, r.reader)}",
            f"  accept odds: {r.accept_pct}%  (objective hash-derived)",
            f"  verdict:     {r.verdict.upper()}",
            f"  confidence:  {r.confidence}/5",
            "",
        ]
    n_reject = len(s.reviews) - s.n_accepts
    lines += [
        "=========================================",
        f" final tally: {s.n_accepts} accept / {n_reject} reject",
        f" area chair says: {s.decision}",
        "=========================================",
    ]
    return "\n".join(lines)


def print_analysis(a: Analysis) -> None:
    print(format_analysis(a))

def print_submission(s: Submission) -> None:
    print(format_submission(s))

def analyze(abstract) -> Analysis:
    out = _OaqAnalysis()
    if isinstance(abstract, (str, Path)) and Path(abstract).exists():
        rc = _lib.oaq_analyze_file(str(abstract).encode("utf-8"), ctypes.byref(out))
    else:
        data = _as_bytes(abstract)
        rc = _lib.oaq_analyze(data, len(data), ctypes.byref(out))
    if rc != 0:
        raise RuntimeError(f"oaq_analyze failed (rc={rc})")
    return Analysis(
        acronym_score=out.acronym_score,
        symbol_score=out.symbol_score,
        lexile_score=out.lexile_score,
        total_score=out.total_score,
        venue=out.venue.decode("utf-8"),
    )

def submit(abstract) -> Submission:
    data = _as_bytes(abstract)
    out = _OaqSubmission()
    rc = _lib.oaq_submit(data, len(data), ctypes.byref(out))
    if rc != 0:
        raise RuntimeError(f"oaq_submit failed (rc={rc})")
    reviews = [
        Review(
            name=out.reviews[i].name.decode("utf-8"),
            reader=_READER.get(out.reviews[i].reader, "unknown"),
            accept_pct=out.reviews[i].accept_pct,
            verdict=_VERDICT.get(out.reviews[i].verdict, "unknown"),
            confidence=out.reviews[i].confidence,
        )
        for i in range(out.n_reviews)
    ]
    return Submission(reviews=reviews, n_accepts=out.n_accepts,
                      decision=out.decision.decode("utf-8"))
