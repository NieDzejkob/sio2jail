#pragma once

#include "SeccompAction.h"

namespace s2j {
namespace seccomp {
namespace action {

class ActionKill : public SeccompAction {
public:
    Type getType() const override;

    tracer::TraceAction execute(tracer::Tracee& tracee) override;

protected:
    virtual uint32_t createLibSeccompAction() const override;
};

}
}
}

