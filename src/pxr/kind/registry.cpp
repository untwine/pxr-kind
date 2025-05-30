// Copyright 2016 Pixar
//
// Licensed under the terms set forth in the LICENSE.txt file available at
// https://openusd.org/license.
//
// Modified by Jeremy Retailleau.

#include "./registry.h"
#include <pxr/plug/plugin.h>
#include <pxr/plug/registry.h>
#include <pxr/tf/instantiateSingleton.h>
#include <pxr/tf/iterator.h>
#include <pxr/tf/stringUtils.h>

namespace pxr {

TF_INSTANTIATE_SINGLETON(KindRegistry);
TF_DEFINE_PUBLIC_TOKENS(KindTokens, KIND_TOKENS);

TF_DEFINE_PRIVATE_TOKENS(_tokens,
    ((PluginKindsKey, "Kinds"))
    );


KindRegistry::KindRegistry()
{
    _RegisterDefaults();
}

KindRegistry::~KindRegistry()
{
    // do nothing
}

/* static */
KindRegistry&
KindRegistry::GetInstance()
{
    return TfSingleton<KindRegistry>::GetInstance();
}

void
KindRegistry::_Register(const TfToken& kind,
                        const TfToken& baseKind)
{
    if (!TfIsValidIdentifier(kind.GetString())) {
        TF_CODING_ERROR("Invalid kind: '%s'", kind.GetText());
        return;
    }

    _KindMap::const_iterator it = _kindMap.find(kind);

    if (it != _kindMap.end()) {
        TF_CODING_ERROR("Kind '%s' has already been registered",
                        kind.GetText());
        return;
    }

    _KindData data;
    data.baseKind = baseKind;
    _kindMap[kind] = data;
}

/* static */ 
bool
KindRegistry::HasKind(const TfToken& kind)
{
    return KindRegistry::GetInstance()._HasKind(kind);
}

bool
KindRegistry::_HasKind(const TfToken& kind) const
{
    return _kindMap.count(kind) != 0;
}

/* static */
TfToken
KindRegistry::GetBaseKind(const TfToken &kind)
{
    return KindRegistry::GetInstance()._GetBaseKind(kind);
}

TfToken
KindRegistry::_GetBaseKind(const TfToken &kind) const
{
    _KindMap::const_iterator it = _kindMap.find(kind);

    if (it == _kindMap.end()) {
        TF_CODING_ERROR("Unknown kind: '%s'", kind.GetText());
        return TfToken();
    }

    return it->second.baseKind;
}

/* static */
bool
KindRegistry::IsA(const TfToken& derivedKind, const TfToken &baseKind)
{
    return KindRegistry::GetInstance()._IsA(derivedKind, baseKind);
}

/* static */
bool
KindRegistry::IsModel(const TfToken &kind)
{
    return IsA(kind, KindTokens->model);
}

/* static */
bool
KindRegistry::IsAssembly(const TfToken &kind)
{
    return IsA(kind, KindTokens->assembly);
}

/* static */
bool
KindRegistry::IsGroup(const TfToken &kind)
{
    return IsA(kind, KindTokens->group);
}

/* static */
bool
KindRegistry::IsComponent(const TfToken &kind)
{
    return IsA(kind, KindTokens->component);
}

/* static */
bool
KindRegistry::IsSubComponent(const TfToken &kind)
{
    return IsA(kind, KindTokens->subcomponent);
}

bool
KindRegistry::_IsA(const TfToken& derivedKind, const TfToken &baseKind) const
{
    if (derivedKind == baseKind) {
        return true;
    }

    _KindMap::const_iterator it = _kindMap.find(derivedKind);

    if (it == _kindMap.end()) {
        // Don't make this a coding error; it's very convenient to allow
        // querying about IsA for any random kind without having to e.g.
        // verify that it's not an empty string first.
        return false;
    }

    if (it->second.baseKind.IsEmpty()) {
        return false;
    }

    return _IsA(it->second.baseKind, baseKind);
}

std::vector<TfToken>
KindRegistry::GetAllKinds()
{
    return KindRegistry::GetInstance()._GetAllKinds();
}

std::vector<TfToken>
KindRegistry::_GetAllKinds() const
{
    std::vector<TfToken> r;
    r.reserve(_kindMap.size());
    for (const auto &entry: _kindMap) {
        r.push_back(entry.first);
    }
    return r;
}

// Helper function to make reading from dictionaries easier
static bool
_GetKey(const JsObject &dict, const std::string &key, JsObject *value)
{
    JsObject::const_iterator i = dict.find(key);
    if (i != dict.end() && i->second.IsObject()) {
        *value = i->second.GetJsObject();
        return true;
    }
    return false;
}


void
KindRegistry::_RegisterDefaults()
{
    // Initialize builtin kind hierarchy.
    _Register(KindTokens->subcomponent);
    _Register(KindTokens->model);
    _Register(KindTokens->component, KindTokens->model);
    _Register(KindTokens->group, KindTokens->model);
    _Register(KindTokens->assembly, KindTokens->group);

    // Check plugInfo for extensions to the kind hierarchy.
    //
    // XXX We only do this once, and do not re-build the kind hierarchy
    //     if someone manages to add more plugins while the app is running.
    //     This allows the KindRegistry to be threadsafe without locking.
    const PlugPluginPtrVector& plugins = 
        PlugRegistry::GetInstance().GetAllPlugins();
    TF_FOR_ALL(plug, plugins){
        JsObject kinds;
        const JsObject &metadata = (*plug)->GetMetadata();
        if (!_GetKey(metadata, _tokens->PluginKindsKey, &kinds)) continue;

        TF_FOR_ALL(kindEntry, kinds){
            // Each entry is a map from kind -> metadata dict.
            TfToken  kind(kindEntry->first);
            JsObject   kindDict;
            if (!_GetKey(kinds, kind, &kindDict)){
                TF_RUNTIME_ERROR("Expected dict for kind '%s'",
                                 kind.GetText());
                continue;
            }

            // Check for baseKind.
            TfToken baseKind;
            JsObject::const_iterator i = kindDict.find("baseKind");
            if (i != kindDict.end()) {
                if (i->second.IsString())  {
                    baseKind = TfToken(i->second.GetString());
                } else {
                    TF_RUNTIME_ERROR("Expected string for baseKind");
                    continue;
                }
            }
            _Register(kind, baseKind);
        }
    }
}

}  // namespace pxr
