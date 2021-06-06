### Essential

- Raw address: an offset in the PE_file.
- Virtual Address (VA): address in the RAM.
- Relative Virtual Address (RVA): the address relative to the `ImageBase`, where the PE is loaded.


### Read EXE File

*get exe file*

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

*load PE in memory*

```C++
void* start_address = load_pe(exe_file_data);
if (start_address)
{
    ((void(*)void) start_address)();    // call entry points
}
```

### Parse PE Header

*parse dos header*

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

*load sections*

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

*set permissions*

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





### Reference

- [Packer Tutorial](https://bidouillesecurity.com/tutorial-writing-a-pe-packer-intro/) - Very good comprehensive Tutorials