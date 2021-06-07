## Essential

* **Raw address** - an offset in the PE_file.
* **Virtual Address (VA)** - address in the RAM.
* **Relative Virtual Address (RVA)** - the address relative to the `ImageBase`, where the PE is loaded.


## Read EXE File

**get exe file**

```C++
FILE* exe_file = fopen(<file>, "rb");
if (!exe_file)
{
    // error
}
fseek(exe_file, 0L, SEEK_END);          // get file size; put pointer at the end
int file_size = ftell(exe_file);
fseek(exe_file, 0L, SEEK_SET);          // put the pointer back at the beginning
char* exe_file_data = malloc(file_size + 1);    // allocate memory and read the whole file
size_t n_read = fread(exe_file_data, 1, file_size, exe_file);   // read the whole file
if (n_read != file_size)
{
    // error
}

// exe_file_data: the read exe-files
```

**load PE in memory**

```C++
void* start_address = load_pe(exe_file_data);
if (start_address)
{
    ((void(*)void) start_address)();    // call entry points
}
```

## Parse PE Header

**parse dos header**

```C++
IMAGE_DOS_HEADER* p_DOS_HDR = (IMAGE_DOS_HEADER*) PE_DATA;
IMAGE_NT_HEADERS* p_NT_HDR = (IMAGE_NT_HEADERS*)(((char*)p_DOS_HDR) + p_DOS_HDR->e_lfanew);

```

*parse nt headers, allocate memory to copy into it*

```C++
DWORD hdr_image_base = p_NT_HDR->OptionalHeader.ImageBase;
DWORD size_of_image = p_NT_HDR->OptionalHeader.SizeOfImage;
DWORD entry_point_RVA = p_NT_HDR->OptionalHeader.AddressOfEntryPoint;
DWORD size_of_headers = p_NT_HDR->OptionalHeader.SizeOfHeaders;

char* ImageBase = char(*)VirtualAlloc(NULL, size_of_image, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
if (ImageBase == NULL)
{
    // error;
}

memcpy(ImageBase, PE_data, size_of_headers);    // copy into memory
```

**load sections**

```C++
IMAGE_SECTION_HEADER* sections = (IMAGE_SECTION_HEADER*)(p_NT_HDR + 1); // casting

for (int i=0; i<p_NT_HDR->FileHeader.NumberOfSections; ++i)
{
    char* dest = Image + sections[i].VirtualAddress;

    if (sections[i].SizeOfRawData > 0)
    {
        memcpy(dest, PE_data + sections[i].PointerToRawData, sections[i].SizeOfRawData);
    }
    else
    {
        memset(dest, 0, sections[i].Misc.VirtualSize);
    }

}

```

**set permissions**

```C++
DWORD oldProtect;
VirtualProtect(ImageBase, p_NT_HDR->OptionalHeader.SizeOfHeaders, PAGE_READONLY, &oldProtect); 

for(int i=0; i<p_NT_HDR->FileHeader.NumberOfSections; ++i) {
    char* dest = ImageBase + sections[i].VirtualAddress;
    DWORD s_perm = sections[i].Characteristics;
    DWORD v_perm = 0; //flags are not the same between virtal protect and the section header
    if(s_perm & IMAGE_SCN_MEM_EXECUTE) {
        v_perm = (s_perm & IMAGE_SCN_MEM_WRITE) ? PAGE_EXECUTE_READWRITE: PAGE_EXECUTE_READ;
    } else {
        v_perm = (s_perm & IMAGE_SCN_MEM_WRITE) ? PAGE_READWRITE: PAGE_READONLY;
    }
    VirtualProtect(dest, sections[i].Misc.VirtualSize, v_perm, &oldProtect);
}
```

**imports**

* Import Directory points to the "Import Directory Table" (IDT)
* Import Address Table Directory points to the "Import Address Table"(IAT)
  
```C++
IMAGE_DATA_DIRECOTRY* data_directory = p_NT_HDR->OptionalHeader.DataDirectory;

IMAGE_IMPORT_DESCRIPTOR* import_descriptors = (IMAGE_IMPORT_DESCRIPTOR*)(ImageBase + data_directory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);  // load the address of the import descriptors array

for (int i=0; import_descriptors[i].OriginalFirstThunk != 0; ++i)
{
    char* module_name = ImageBase + import_descriptors[i].Name;
    HMODULE import_module = LoadLibraryA(module_name);
    if (import_module == NULL)
    {
        return NULL;
    }

    IMAGE_THUNK_DATA* lookup_table = (IMAGE_THUNK_DATA*)(ImageBase + import_descriptors[i].OriginalFirstThunk); // IDT

    IMAGE_THUNK_DATA* address_table = (IMAGE_THUNK_DATA*)(ImageBase + import_descriptors[i].FirstThunk);    // IAT

    for (int i=0; lookup_table[i].u1.AddressOfData != 0; ++i)
    {
        void* function_handle = NULL;

        DWORD lookup_addr = lookup_table[i].u1.AddressOfData;

        if ((lookup_addr & IMAGE_ORDINAL_FLAG) == 0)        // if first bit is not 1
        {
            IMAGE_IMPORT_BY_NAME* image_import = (IMAGE_IMPORT_BY_NAME*)(ImageBase + lookup_addr);

            char* funct_name = (char*)&(image_import->Name);

            function_handle = (void*)GetProcAddress(import_module, funct_name); // get func address from module using name
        }
        else
        {
            function_handle = (void*)GetProcAddress(import_module, (LPSTR)lookup_addr); // import by ordinal directly
        }
    }

}
```

**relocations**

```C++
DWORD delta_VA_reloc = ((DWORD)ImageBase) - p_NT_HDR->OptionalHeader.ImageBase

if (data_directory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress != 0 && delta_VA_reloc != 0)
{
    IMAGE_BASE_RELOCATION* p_reloc = (IMAGE_BASE_RELOCATION*)(ImageBase + data_directory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress);  // calculate the relocation table address

    while(p_reloc->VirtualAddress != 0)
    {
        DWORD size = (p_reloc->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION))/2;

        WORD* reloc = (WORD*)(p_reloc + 1);
        for(int i=0; i<size; ++i)
        {
            int type = reloc[i] >> 12;  // type is the first 4 bits of the relocation word
            int offset = reloc[i] & 0x0fff;     // offset is the last 12 bits
            DWORD* change_addr = (DWORD*)(ImageBase + p_reloc->VirtualAddress + offset);
            switch(type)
            {
                case IMAGE_REL_BASED_HIGHLOW:
                    *change_addr += delta_VA_reloc;
                    break;
                default:
                    break
            }
        }
        
        p_reloc = (IMAGE_BASE_RELOCATION*)(((DWORD)p_reloc) + p_reloc->SizeOfBlock);    // switch to the next relocation block, base on the size
    }
}

```

## Unpacker.exe

```C++
int _start(void)
{
    char* unpacker_VA = (char*)GetModuleHandleA(NULL);  // Get the current module VA

    IMAGE_DOS_HEADER* p_DOS_HDR = (IMAGE_DOS_HEADER*)unpacker_VA;
    IMAGE_NT_HEADERS* p_NT_HDR = (IMAGE_NT_HEADER*)(((char*)p_DOS_HDR) + p_DOS_HDR->e_lfanew);
    IMAGE_SECTION_HEADER* sections = (IMAGE_SECTION_HEADER*)(p_NT_HDR + 1);

    char* packed_PE = NULL;
    char packed_section_name[] = ".packed";

    for (int i=0; i<p_NT_HDR->FileHeader.NumberOfSections; ++i)
    {
        if (mystrcmp(sections[i].Name, packed_section_name))
        {
            packed_PE = unpacker_VA + sections[i].VirtualAddress;
            break;
        }
    }   

    if (packed_PE != NULL)
    {
        void (*packed_entry_point)(void) = (void(*)()load_PE(packed_PE));
        packed_entry_point();
    }
}

```

```
mingw32-gcc.exe unpack.c -o unpacker.exe "-Wl, --entry=__start" -nostartfiles -nostdlib -lkernel32
```

## Packer

use [lief](https://lief.quarkslab.com/doc/stable/) library to pack unpacker.exe to an executable




## Reference

- [Packer Tutorial](https://bidouillesecurity.com/tutorial-writing-a-pe-packer-intro/) - Very good comprehensive Tutorials