#script(python)
import clingo
from clingo import *

def on_model(m):
        print ", ".join(map(str, m.symbols(extra=True, shown=True)))


def main(prg):
  prg.ground([("base",[])])
  prg.solve(on_model=on_model)
#end.

#include <csp>.
&dom{1..10} = x.

{a}.

&sum{x} = 5 :- a.

&show{x : a}.
