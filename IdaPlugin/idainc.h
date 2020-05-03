#ifndef __IDAINC_H__
#define __IDAINC_H__

#pragma warning(push)
// disable warnings "implemented" in IDA headers
#pragma warning(disable : 4512)
#pragma warning(disable : 4244)
#pragma warning(disable : 4018)
#pragma warning(disable : 4127)
#pragma warning(disable : 4201)
#pragma warning(disable : 4267)
#pragma warning(disable : 4530)

#include <ida.hpp>
#include <idp.hpp>
#include <idd.hpp>
#include <dbg.hpp>
#include <expr.hpp>
#include <auto.hpp>
#include <bytes.hpp>
#include <loader.hpp>
#include <kernwin.hpp>
#include <allins.hpp>
#pragma warning(pop)

#endif // __IDAINC_H__