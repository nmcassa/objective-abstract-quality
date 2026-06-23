import oaq

a = oaq.analyze("../abstracts/nick.txt")
oaq.print_analysis(a)

print()

s = oaq.submit("../abstracts/nick.txt")
oaq.print_submission(s)
