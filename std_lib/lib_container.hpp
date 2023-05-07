//
// Created by Dogs-Cute on 5/7/2023.
//

#ifndef POURUPSCRIPTINTERP_LIB_CONTAINER_HPP
#define POURUPSCRIPTINTERP_LIB_CONTAINER_HPP

#include "../pups.h"

namespace PUPS::Std_Container {
    template<typename Container>
    class INST_Container : public InstanceBase {
        friend class TP_Contains;

        ObjectPtr container;
        ObjectPtr element;
    };

    class TP_Container : public TypeBase {
    protected:
        ObjectPtr construct(PUPS::Scope *scope, PUPS::Report &report, std::queue<Token> &names,
                            std::queue<Token> &args) override {

        }
    };

    void INCLUDE() {

    }
}

#endif //POURUPSCRIPTINTERP_LIB_CONTAINER_HPP
