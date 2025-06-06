// Copyright 2017 Pixar
//
// Licensed under the terms set forth in the LICENSE.txt file available at
// https://openusd.org/license.
//
// Modified by Jeremy Retailleau.

#ifndef PXR_KIND_API_H
#define PXR_KIND_API_H

#include <pxr/arch/export.h>

#if defined(PXR_STATIC)
#   define KIND_API
#   define KIND_API_TEMPLATE_CLASS(...)
#   define KIND_API_TEMPLATE_STRUCT(...)
#   define KIND_LOCAL
#else
#   if defined(KIND_EXPORTS)
#       define KIND_API ARCH_EXPORT
#       define KIND_API_TEMPLATE_CLASS(...) ARCH_EXPORT_TEMPLATE(class, __VA_ARGS__)
#       define KIND_API_TEMPLATE_STRUCT(...) ARCH_EXPORT_TEMPLATE(struct, __VA_ARGS__)
#   else
#       define KIND_API ARCH_IMPORT
#       define KIND_API_TEMPLATE_CLASS(...) ARCH_IMPORT_TEMPLATE(class, __VA_ARGS__)
#       define KIND_API_TEMPLATE_STRUCT(...) ARCH_IMPORT_TEMPLATE(struct, __VA_ARGS__)
#   endif
#   define KIND_LOCAL ARCH_HIDDEN
#endif

#endif
