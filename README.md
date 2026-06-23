# objective-abstract-quality

This repo provides code that with two modes: submission mode and analysis mode. Both modes attempt to give an objective quality score to an abstract (just from different perspectives).

## submission mode

Given an abstract from an input text file. Four reviewers will objectively review your paper and give you either an accept or deny (along with a confidence review). It is likely that the assigned reviewer will not actually review the paper, there is a chance that one of the assigned reviewers students will review the paper (and there is a chance he will not find time to read the paper at all). 

The reviewing system is purely objective. Each reviewer will compute a long hash that is used to generate a random integer between 1 and 99. This integer will be used to represent the likelihood of getting an accept, only by flipping a weighted coin will that reviewer make a decision. Confidence is random as well. 

## analysis mode

Given an abstract from an input test file. Our advanced AI will objectively grade the quality of an abstract via three key metrics: (1) lexile score (comprehension difficultly), (2) amount of numeric and symbol characters, and (3) amount of acronyms.

The output will present you with key analysis such as: (1) a prediction of the conference quality (conference rankings) and (2) a summary of above metrics
