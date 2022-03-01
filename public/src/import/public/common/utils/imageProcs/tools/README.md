# paktools
A collection of python tools to build an hardware flash pak and work with existing paks
  
## pakbuild
The official tool for building paks for flash.  It uses an input manifest file that 
 describes all the files to be included in the pak.  It includes verification and 
 validation of the output pak.
  
## paktool
Provided to interogate/manipulate the contents of a pak.  Do not use this tool to
 generate a pak, use `pakbuild`.  This tool gives you plenty of ammo to shoot yourself
 in the foot.
  
Provided functions:
- `add` : insert a file
- `hash` : generate hash values for the files in the image
- `extract` : extract files from the archive
- `list` : list the files in the archive
- `remove` : remove a file
