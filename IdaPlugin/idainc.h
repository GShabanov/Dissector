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

#pragma warning(disable : 26432)
#pragma warning(disable : 26433)
#pragma warning(disable : 26440)
#pragma warning(disable : 26443)
#pragma warning(disable : 26455)
#pragma warning(disable : 26461)
#pragma warning(disable : 26477)
#pragma warning(disable : 26493)
#pragma warning(disable : 26495)
#pragma warning(disable : 26496)
#pragma warning(disable : 26408)


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