#pragma once

namespace ark::net
{
    using flag_type = uint32_t;
    using id_type = uint32_t;
    using link_type = uint16_t;

    void init();
    void destroy();
}
