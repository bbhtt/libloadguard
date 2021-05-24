Audit library to help Flatpak applications
using Freedesktop SDK runtime to cope with
badly behaving programs. Reads a list of
pairs from "/etc/freedesktop-sdk.ld.so.blockedlist".
Config file can be overridden at build time through
-DSHARED_LIBRARY_GUARD_CONFIG or at run time using
the environment variable SHARED_LIBRARY_GUARD_CONFIG


Versioning follows YY.MM.release


Original sources available from
https://gitlab.com/freedesktop-sdk/freedesktop-sdk/merge_requests/496