# tests/test_parity.py
import subprocess, re, oaq

def _cli_scores(path):
    out = subprocess.check_output(["./oaq", "analysis", path], text=True)
    grab = lambda label: int(re.search(rf"{label}:\s+(\d+)", out).group(1))
    return grab("acronym score"), grab("symbol score"), grab("lexile score")

def test_cli_matches_python():
    path = "abstracts/nick.txt"
    a = oaq.analyze(path)
    assert _cli_scores(path) == (a.acronym_score, a.symbol_score, a.lexile_score)
