# Windows Projected File System (ProjFS)

- Doc: https://docs.microsoft.com/en-us/windows/win32/projfs/projected-file-system

- ProjFS comes as optional feature on Windows. It must be enabled: Programs and Features -> Turn Windows features on or off -> Windows Projected File System

- At this point the code fails to assign virtualization instance id, as a target directory has a reparse point.
  - TODO: How to remove reparse point from a target directory?

# Schell related functions

- Doc: https://docs.microsoft.com/en-us/windows/win32/api/shlobj_core/nf-shlobj_core-shchangenotifyregister

- Not sure if these family of functions provide a feature to monitor file opening events. At least I haven't found a way to obtain these events.