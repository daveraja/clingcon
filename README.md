Clingcon extends the possibilities of Clingo by (mostly linear) constraints over Integers.
For the syntax of constraint logic programs please see the examples folder.
Use the theory language description file csp.lp to enable constraint parsing.

Features:

 * based on clingo 5.x and libcsp 1.x
 * lazy nogood generation based on the order encoding
 * lazy variable generation allows for huge domains
 * supports multi-shot solving
 * supports optimization
 * no blackbox csp libraries used
 * uses gringo-5.x theory language interface, please include "csp.lp"
 * use --help=2 to find specific options for constraint solving 

Call clingcon --help=2 for further options on constraint processing.
Please consult the following resources for further information:

  - CHANGES:  changes between different releases
  - INSTALL:  installation instructions and software requirements
  - examples: a folder with examples each having a focus on certain features

For more information please visit the project website: 
  
  http://potassco.org/

[![Build Status master](https://badges.herokuapp.com/travis/potassco/clingcon?branch=master&label=master)](https://travis-ci.org/potassco/clingcon?branch=master)
