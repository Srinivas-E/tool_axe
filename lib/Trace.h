// Copyright (c) 2011, Richard Osborne, All rights reserved
// This software is freely distributable under a derivative of the
// University of Illinois/NCSA Open Source License posted in
// LICENSE.txt and at <http://github.xcore.com/>

#ifndef _Trace_h_
#define _Trace_h_

#include "Config.h"
#include "SymbolInfo.h"
#include "Register.h"
#include "TerminalColours.h"
#include <iostream>
#include <sstream>
#include <string>
#include <memory>

namespace axe {

class EventableResource;
class SystemState;
class Node;
class Thread;

inline std::ostream &operator<<(std::ostream &out, const Register::Reg &r) {
  return out << getRegisterName(r);
}

class Tracer {
private:
  struct LineState {
    LineState(std::ostringstream *b) :
      thread(0),
      out(0),
      buf(b),
      pending(0) {}
    LineState(std::ostream &o, std::ostringstream &b, std::ostringstream &pendingBuf) :
      thread(0),
      out(&o),
      buf(&b),
      pending(&pendingBuf) {}
    const Thread *thread;
    bool hadRegWrite;
    size_t numEscapeChars;
    std::ostream *out;
    std::ostringstream *buf;
    std::ostringstream *pending;
  };
  class PushLineState {
  private:
    bool needRestore;
    LineState line;
    Tracer &parent;
  public:
    PushLineState(Tracer &parent);
    ~PushLineState();
    bool getRestore() const { return needRestore; }
  };
  bool tracingEnabled;
  std::ostringstream buf;
  std::ostringstream pendingBuf;
  LineState line;
  SymbolInfo symInfo;
  TerminalColours colours;

  void escapeCode(const char *s);
  void reset() { escapeCode(colours.reset); }
  void red() { escapeCode(colours.red); }
  void green() { escapeCode(colours.green); }

  void printThreadName();
  void printCommonStart();
  void printCommonStart(const Node &n);
  void printCommonStart(const Thread &t);
  void printCommonEnd();
  void printThreadPC();

  void printImm(uint32_t op)
  {
    *line.buf << op;
  }

  void printSrcRegister(Register::Reg op);
  void printDestRegister(Register::Reg op);
  void printSrcDestRegister(Register::Reg op);
  void printCPRelOffset(uint32_t op);
  void printDPRelOffset(uint32_t op);

  void dumpThreadSummary(const Core &core);
  void dumpThreadSummary(const SystemState &system);

  void syscallBegin(const Thread &t);
public:
  Tracer(bool tracing) :
    tracingEnabled(tracing),
    line(std::cout, buf, pendingBuf),
    colours(TerminalColours::null) {}
  bool getTracingEnabled() const { return tracingEnabled; }
  SymbolInfo *getSymbolInfo() { return &symInfo; }
  void setColour(bool enable);

  void instructionBegin(const Thread &t);

  void regWrite(Register::Reg reg, uint32_t value);

  void instructionEnd() {
    printCommonEnd();
  }

  void SSwitchRead(const Node &node, uint32_t retAddress, uint16_t regNum);
  void SSwitchWrite(const Node &node, uint32_t retAddress, uint16_t regNum,
                    uint32_t value);
  void SSwitchNack(const Node &node, uint32_t dest);
  void SSwitchAck(const Node &node, uint32_t dest);
  void SSwitchAck(const Node &node, uint32_t data, uint32_t dest);

  void exception(const Thread &t, uint32_t et, uint32_t ed,
                 uint32_t sed, uint32_t ssr, uint32_t spc);

  void event(const Thread &t, const EventableResource &res, uint32_t pc,
             uint32_t ev);

  void interrupt(const Thread &t, const EventableResource &res, uint32_t pc,
                 uint32_t ssr, uint32_t spc, uint32_t sed, uint32_t ed);

  void syscall(const Thread &t, const std::string &s) {
    syscallBegin(t);
    *line.buf << s << "()";
    reset();
    printCommonEnd();
  }
  template<typename T0>
  void syscall(const Thread &t, const std::string &s,
               T0 op0) {
    syscallBegin(t);
    *line.buf << s << '(' << op0 << ')';
    reset();
    printCommonEnd();
  }
  void timeout(const SystemState &system, ticks_t time);
  void noRunnableThreads(const SystemState &system);
};

} // End axe namespace

#endif //_Trace_h_
