//
// Created by Nemo li on 2024/2/19.
//

#include "light.h"

RTTR_REGISTRATION {
    rttr::registration::class_<Light>("Light")
            .constructor<>()(rttr::policy::ctor::as_raw_ptr);
}