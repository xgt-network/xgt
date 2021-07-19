#include <stdint.h>
#include <xgt/utilities/git_revision.hpp>

#define XGT_GIT_REVISION_SHA "30ccdd98a05bbc4c7393868627dc569eb532ac66"
#define XGT_GIT_REVISION_UNIX_TIMESTAMP 1625816327
#define XGT_GIT_REVISION_DESCRIPTION "1.3.1-16-g30ccdd9"

namespace xgt { namespace utilities {

const char* const git_revision_sha = XGT_GIT_REVISION_SHA;
const uint32_t git_revision_unix_timestamp = XGT_GIT_REVISION_UNIX_TIMESTAMP;
const char* const git_revision_description = XGT_GIT_REVISION_DESCRIPTION;

} } // end namespace xgt::utilities
