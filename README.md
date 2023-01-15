# import-hoodini
Simple runtime import protection &amp; hook mitigation


Import Hoodini is a simple concept of reversing the common usage of the Import Address Table.
The IAT (Import Address Table) is used in every native Windows application to allow modules to import routines which have been exported by other libraries/modules. Attackers will often abuse the IAT by either swapping the pointer to their own prologue OR by hooking the exported routine. This project mitigates and prevents these types of attacks by registering callbacks using small assembly stubs relative to every import which is responsbible for integrity checking them before allowing the call to commence. A snippet of this stub can be found below:
[CODE]
		0:  48 81 ec 88 00 00 00    sub    rsp,0x88                     ; allocating stack space
		7:  48 89 4c 24 28          mov    QWORD PTR [rsp+0x28],rcx     ; 'saving' the stack so that the original call can commence
		c:  48 89 54 24 30          mov    QWORD PTR [rsp+0x30],rdx
		11: 4c 89 44 24 38          mov    QWORD PTR [rsp+0x38],r8
		16: 4c 89 4c 24 40          mov    QWORD PTR [rsp+0x40],r9
		1b: 0f 11 44 24 48          movups XMMWORD PTR [rsp+0x48],xmm0
		20: 0f 11 4c 24 58          movups XMMWORD PTR [rsp+0x58],xmm1
		25: 0f 11 54 24 68          movups XMMWORD PTR [rsp+0x68],xmm2
		2a: 0f 11 5c 24 78          movups XMMWORD PTR [rsp+0x78],xmm3
    
		2f: 48 b9 fe ca be ba 00    movups rcx,0xbabecafe ; setting a1 (rcx) to the import's pointer
		36: 00 00 00
		39: 48 8d 54 24 28          lea    rdx,[rsp+0x28]
		3e: 48 b8 ef be ad de 00    movabs rax,0xdeadbeef               ; call our 'callback' which handles integrity
		45: 00 00 00
    48: ff d0                   call   rax                        
		
    4a: 48 8b 4c 24 28          mov    rcx,QWORD PTR [rsp+0x28]     ; restore the stack
		4f: 48 8b 54 24 30          mov    rdx,QWORD PTR [rsp+0x30]
		54: 4c 8b 44 24 38          mov    r8,QWORD PTR [rsp+0x38]
		59: 4c 8b 4c 24 40          mov    r9,QWORD PTR [rsp+0x40]
		5e: 0f 10 44 24 48          movups xmm0,XMMWORD PTR [rsp+0x48]
		63: 0f 10 4c 24 58          movups xmm1,XMMWORD PTR [rsp+0x58]
		68: 0f 10 54 24 68          movups xmm2,XMMWORD PTR [rsp+0x68]
		6d: 0f 10 5c 24 78          movups xmm3,XMMWORD PTR [rsp+0x78]
		72: 49 ba fe ca be ba 00    movups r10,0xbabecafe
		79: 00 00 00
		7c: 48 81 c4 88 00 00 00    add    rsp,0x88                  
		83: 41 ff e2                jmp    r10                          ; call original export / imported routine
[/CODE]



Usage of this project is very simple. Simply include "import-hoodini.hpp" and pick one of the following setups:

A generic project may do the following which will simply protect ALL imported routines:
[CODE]
ImportHoodini::Setup_AllImports();
ImportHoodini::ActivateImportCallbacks();
[/CODE]

OR if you wish to specify a list of imports which should NOT be protected:
[CODE]
// Basic refuse list for printf()
std::vector<std::uint64_t> RefuseList = {
	(std::uint64_t)&__stdio_common_vfprintf,
	(std::uint64_t)&__acrt_iob_func
};

ImportHoodini::Setup_AllImports(
  GetModuleHandleA(NULL),
  RefuseList
);

ImportHoodini::ActivateImportCallbacks();
[/CODE]

OR if you wish to specify a list of imports which should ONLY be protected:
[CODE]
// ONLY protect IsDebuggerPresent
std::vector<std::uint64_t> ProtectionList = {
  (std::uint64_t)&IsDebuggerPresent
};

ImportHoodini::Setup_Specific(
	GetModuleHandleA(NULL),
	ProtectionList
);
[/CODE]
