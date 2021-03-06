// Copyright (c) 2012, Richard Osborne, All rights reserved
// This software is freely distributable under a derivative of the
// University of Illinois/NCSA Open Source License posted in
// LICENSE.txt and at <http://github.xcore.com/>

#ifndef _XEReader_h
#define _XEReader_h

#include <memory>

namespace axe {

class XE;
class SystemState;
class PortAliases;
class Tracer;

class XEReader {
  XE &xe;
public:
  XEReader(XE &x) : xe(x) {}
  std::auto_ptr<SystemState>
    readConfig(std::auto_ptr<Tracer> tracer = std::auto_ptr<Tracer>());
  void readPortAliases(PortAliases &aliases);
};

} // End axe namespace

#endif // _XEReader_h
