#pragma once

#include <signatures_template.h>

#ifdef _WIN32

#define CASHook_CASHook_Signature "\x8A\x44\x24\x2A\x2A\x54\x24\x2A\x2A\x8A\x5C\x24\x2A\x2A\x8B\xF1\xB9"
#define CASHook_Call_Signature "\x8B\x4C\x24\x04\x8D\x44\x24\x0C\x50\xFF\x74\x24\x0C\x6A\x00\xE8"
#define CString_Assign_Signature "\x8B\x44\x24\x04\x85\xC0\x2A\x2A\x2A\x2A\x2A\x2A\x2A\x2A\x2A\x0F\x2A\x2A\x8B\x2A\x14"
#define CString_dtor_Signature "\x8B\x51\x14\x3B\xD1\x2A\x2A\x8B\x49\x1C\x8B\xC1\xC1\xE8\x1F"

#define CASDocumentation_RegisterObjectType_Signature "\x68\x01\x00\x04\x00\x6A\x00\x68\x2A\x2A\x2A\x2A\x68\x2A\x2A\x2A\x2A\x8B\xCE\xE8"
#define CASDocumentation_RegisterObjectProperty_Signature "\x6A\x00\x68\x2A\x2A\x2A\x2A\x68\x2A\x2A\x2A\x2A\x68\x2A\x2A\x2A\x2A\x8B\xCE\xE8\x2A\x2A\x2A\x2A\x6A\x04"
#define CASDocumentation_RegisterGlobalProperty_Signature "\xFF\x70\x28\x8B\x2A\x68\x2A\x2A\x2A\x2A\x68\x2A\x2A\x2A\x2A\xE8\x2A\x2A\x2A\x2A\x8B\x2A\x2A\x64\x89\x0D\x00\x00\x00\x00"
#define CASDocumentation_RegisterObjectMethod_Signature "\x50\x68\x2A\x2A\x2A\x2A\x68\x2A\x2A\x2A\x2A\x68\x2A\x2A\x2A\x2A\xE8\x2A\x2A\x2A\x2A\x6A\x03"
#define CASDocumentation_RegisterObjectBehaviour_Signature "\x50\x68\x2A\x2A\x2A\x2A\x6A\x02\x68\x2A\x2A\x2A\x2A\x68\x2A\x2A\x2A\x2A\xE8\x2A\x2A\x2A\x2A\x6A\x04\x8D"
#define CASDocumentation_RegisterFuncDef_Signature "\xE8\x2A\x2A\x2A\x2A\x68\x01\x00\x04\x00\x6A\x1C"
#define CASDocumentation_RegisterEnum_Signature "\x6A\x00\x68\x2A\x2A\x2A\x2A\x68\x2A\x2A\x2A\x2A\x2A\x2A\xE8\x2A\x2A\x2A\x2A\x6A\x01"
#define CASDocumentation_RegisterEnumValue_Signature "\x6A\x01\x68\x2A\x2A\x2A\x2A\x68\x2A\x2A\x2A\x2A\x68\x2A\x2A\x2A\x2A\x2A\x2A\xE8\x2A\x2A\x2A\x2A\x6A\x02\x68\x2A\x2A\x2A\x2A\x68\x2A\x2A\x2A\x2A\x68\x2A\x2A\x2A\x2A\x2A\x2A\xE8\x2A\x2A\x2A\x2A\x6A\x04"

#define CASDirectoryList_CreateDirectory_Signature "\x6A\x07\x6A\x00\x6A\x03\x6A\x01\x68\x2A\x2A\x2A\x2A\x8B\x2A\xE8"
#define CASFunction_Create_Signature "\xC6\x45\x2A\x01\xA3\x2A\x2A\x2A\x2A\x6A\x01\xFF\x70\x2A\xFF\x75\x2A\xE8"
#define CASBaseCallable_Call_Signature "\x6A\x00\x2A\x2A\x2A\x6A\x00\xFF\x2A\x0C\xE8"
#define CASRefCountedBaseClass_InternalRelease_Signature "\x8B\x2A\x0C\x85\xFF\x2A\x2A\x8D\x4F\x04\xE8\x2A\x2A\x2A\x2A\x84\xC0"
#define CScriptAny_Release_Signature "\xC6\x2A\x2A\x01\x8B\x4D\xD0\xE8\x2A\x2A\x2A\x2A\x8A\x2A\x5C"
#define CScriptArray_Release_Signature "\xE8\x2A\x2A\x2A\x2A\xC7\x2A\x08\x00\x00\x00\x00\x2A\xC3"

#define g_pServerManager_Signature "\xC6\x45\x2A\x01\xA3\x2A\x2A\x2A\x2A\x6A\x01\xFF\x70\x2A\xFF\x75\x2A\xE8"

#else

#define CASHook_CASHook_Signature "\xC7\x44\x24\x2A\x03\x00\x00\x00\xC7\x44\x24\x2A\x02\x00\x00\x00\x8D\x2A\x2A\x2A\x2A\x2A\x89\x0C\x2A\xE8"
#define CASHook_CASHook_Symbol "_ZN7CASHookC2EhhPKcS1_S1_RK16CASHookArguments"

#define CASHook_Call_Signature "\x83\xEC\x1C\x8D\x44\x24\x28\x89\x44\x24\x0C\x8B\x54\x24\x24\x89\x54\x24\x08\xC7\x44\x24\x04\x00\x00\x00\x00"
#define CASHook_Call_Symbol "_ZN7CASHook4CallEiz"

#define CString_Assign_Signature "\xC7\x44\x24\x2A\x00\x02\x00\x00\x8D\x2A\x24\x10\x89\x2A\x2A\xE8\x2A\x2A\x2A\x2A\x89\x2A\x24\x08\x89\x2A\x24\x04\x89\x2A\x24\xE8"
#define CString_Assign_Symbol "_ZN7CString6AssignEPKcj"

#define CString_dtor_Signature "\x56\x53\x83\xEC\x14\xE8\x2A\x2A\x2A\x2A\x81\x2A\x2A\x2A\x2A\x2A\x8B\x74\x24\x2A\x8B\x46\x14\x39\xF0\x74\x2A\x8B\x56\x1C"
#define CString_dtor_Symbol "_ZN7CStringD2Ev"

#define CASDocumentation_RegisterObjectType_Signature "\xC7\x44\x24\x2A\x01\x00\x04\x00\xC7\x44\x24\x2A\x00\x00\x00\x00\x8D\x2A\x2A\x2A\x2A\x2A\x89\x2A\x2A\x2A\x8D\x2A\x2A\x2A\x2A\x2A\x89\x2A\x2A\x2A\x8B\x2A\x2A\x2A\x2A\x00\x00\x89\x04\x24\xE8"
#define CASDocumentation_RegisterObjectType_Symbol "_ZN16CASDocumentation18RegisterObjectTypeEPKcS1_im"

#define CASDocumentation_RegisterObjectProperty_Signature "\xE8\x2A\x2A\x2A\x2A\xC7\x44\x24\x2A\x44\x02\x00\x00"
#define CASDocumentation_RegisterObjectProperty_Symbol "_ZN16CASDocumentation22RegisterObjectPropertyEPKcS1_S1_i"

#define CASDocumentation_RegisterGlobalProperty_Signature "\x8B\x2A\x2A\x2A\x89\x04\x24\x89\x2A\xE8\x2A\x2A\x2A\x2A\x83\xC4\x5C\x2A\x2A\x2A\x2A\xC3"
#define CASDocumentation_RegisterGlobalProperty_Symbol "_ZN16CASDocumentation22RegisterGlobalPropertyEPKcS1_Pv"

#define CASDocumentation_RegisterObjectMethod_Signature "\xE8\x2A\x2A\x2A\x2A\x8D\x2A\x2A\x2A\x2A\x2A\x89\x2A\x2A\x2A\x00\x00\x00\xC7\x2A\x2A\x2A\x00\x00\x00\x00\x00\x00\x00\x8D\x2A\x01\xC6\x01\x00"
#define CASDocumentation_RegisterObjectMethod_Symbol "_ZN16CASDocumentation20RegisterObjectMethodEPKcS1_S1_RK10asSFuncPtrm"

#define CASDocumentation_RegisterObjectBehaviour_Signature "\xC7\x44\x24\x2A\x03\x00\x00\x00\x89\x2A\x2A\x2A\x8D\x2A\x2A\x2A\x2A\x2A\x89\x2A\x2A\x2A\x89\x2A\x2A\xE8"
#define CASDocumentation_RegisterObjectBehaviour_Symbol "_ZN16CASDocumentation23RegisterObjectBehaviourEPKcS1_13asEBehavioursS1_RK10asSFuncPtrmPv"

#define CASDocumentation_RegisterFuncDef_Signature "\xE8\x2A\x2A\x2A\x2A\x89\x2A\x2A\xE8\x2A\x2A\x2A\x2A\xC7\x44\x24\x2A\x03\x00\x00\x00\x89\x2A\x2A\xE8\x2A\x2A\x2A\x2A\x89\x2A\x2A\x2A\xC7\x44\x24\x2A\x01\x00\x04\x00"
#define CASDocumentation_RegisterFuncDef_Symbol "_ZN16CASDocumentation15RegisterFuncDefEPKcS1_"

#define CASDocumentation_RegisterEnum_Signature "\xC7\x44\x24\x2A\x00\x00\x00\x00\x89\x2A\x2A\x2A\x8D\x2A\x2A\x2A\x2A\x2A\x89\x2A\x2A\x2A\x89\x2A\x2A\xE8\x2A\x2A\x2A\x2A\xC7\x44\x24\x2A\x01\x00\x00\x00"
#define CASDocumentation_RegisterEnum_Symbol "_ZN16CASDocumentation12RegisterEnumEPKcS1_NS_9ENUM_TYPEE"

#define CASDocumentation_RegisterEnumValue_Signature "\x89\x44\x24\x04\x89\x3C\x24\xE8\x2A\x2A\x2A\x2A\xC7\x44\x24\x2A\x02\x00\x00\x00"
#define CASDocumentation_RegisterEnumValue_Symbol "_ZN16CASDocumentation17RegisterEnumValueEPKcS1_S1_i"

#define CASDirectoryList_CreateDirectory_Signature "\xE8\x2A\x2A\x2A\x2A\xC7\x44\x24\x2A\x07\x00\x00\x00\xC7\x44\x24\x2A\x00\x00\x00\x00\xC7\x44\x24\x2A\x03\x00\x00\x00\xC7\x44\x24\x2A\x01\x00\x00\x00"
#define CASDirectoryList_CreateDirectory_Symbol "_ZN16CASDirectoryList15CreateDirectoryEPKchhhh"

#define CASFunction_Create_Signature "\xE8\x2A\x2A\x2A\x2A\x89\x2A\x2A\x8D\x2A\x2A\x89\x2A\x2A\xE8\x2A\x2A\x2A\x2A\xC7\x2A\x2A\x00\x00\x00\x00\xC7\x2A\x2A\x00\x00\x00\x00\xC7\x2A\x2A\x00\x00\x00\x00\xC7\x2A\x2A\x07\x00\x00\x00"
#define CASFunction_Create_Symbol "_ZN11CASFunction6CreateEP17asIScriptFunctionP9CASModuleb"

#define CASBaseCallable_Call_Signature "\xD9\x2A\x24\x2A\xDD\x2A\x24\x2A\x89\x2A\x2A\x2A\xC7\x44\x24\x2A\x00\x00\x00\x00\x89\x2A\x2A\xE8\x2A\x2A\x2A\x2A\x83\xC4\x28"
#define CASBaseCallable_Call_Symbol "_ZN15CASBaseCallable4CallEiz"

#define CASRefCountedBaseClass_InternalRelease_Signature "\x89\x04\x24\xE8\x2A\x2A\x2A\x2A\x84\xC0\x2A\x2A\x89\x2A\x83\xC4\x2C"
#define CASRefCountedBaseClass_InternalRelease_Symbol "_ZNK22CASRefCountedBaseClass15InternalReleaseEv"

#define CScriptAny_Release_Signature "\xE8\x2A\x2A\x2A\x2A\xC7\x2A\xAC\x00\x00\x00\x00\x00\x00\x00\x83\xC4\x14"
#define CScriptAny_Release_Symbol "_ZNK10CScriptAny7ReleaseEv"

#define CScriptArray_Release_Signature "\xC7\x44\x24\x2A\x00\x00\x00\x00\x89\x2A\x2A\x89\x2A\xE8\x2A\x2A\x2A\x2A\x89\x2A\x2A\xE8\x2A\x2A\x2A\x2A\x83\xC4\x2C"
#define CScriptArray_Release_Symbol "_ZNK12CScriptArray7ReleaseEv"

#define g_pServerManager_Symbol "_ZZN16CASServerManager11GetInstanceEvE9pInstance"

#endif