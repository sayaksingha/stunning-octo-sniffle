#include "util.hpp"

static error err_t2error_keep(Prosody_Test::err_t err)
{
 if (err) return error(err_t2error(Prosody_Test::error_cause(err)), Prosody_Test::error_text(err));
 return error();
}

error err_t2error(Prosody_Test::err_t err)
{
 error e(err_t2error_keep(err));
 Prosody_Test::error_dtor(err);
 return e;
}
