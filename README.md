# YAC8E
Yet Another Chip-8 Emulator

# Purpose
This is a learning project for me (Cristei Gabriel-Marian [@cristeigabriel]), meant to help others understand the Chip-8 architecture and inner workings while I do the same, but through direct work and experimenting, which I recommend that all likewise interested people do.

This project will include documentation to the topmost of my knowledge and of others within my ability and of the (possible) contributors to this project.

# Next Milestone
I'm not always available, so I can't provide this exactly now, but I'm working on a DirectX9 renderer for this project and my others. This'll, obviously, take a while, so until it's done I'll probably not commit much, considering most of the emulator, as of 09/03/2021, is mostly finished (outside of instructions relevant to the aforementioned, and ROM parsing, which are both do-able in little time, unlike a renderer, and perhaps audio handling!).

# Platform
YAC8E is officially developed on Windows, providing an out of the box ready experience for Visual Studio Community (2019) users, working on the most default of toolchains.

YAC8E MIGHT be ported to Linux one day, but it is not yet confirmed or in plans.

# Customization
YAC8E allows for chip core elements and delimiters to be customized widely, but it is not a great idea to take upon it without knowledge of what you're doing, so watch out.

# Support
To see what instructions YAC8E supports and currently has, refer [here](https://github.com/cristeigabriel/YAC8E/blob/main/chip-8/chip-8.cpp#L13).

# Debugging
YAC8E under Debug mode debugs by hand, and prnits out the initialization process, every computed byte, and every explicitly computed address alongside the pre-action PC, and how the action would be, out, under syntax form.

# Licensing
Do whatever the fuck you want with this, but put me (and contributors) somewhere there if you plan on using my project. Also, I'm not eligible to be held accountable for anything that you might be doing with this. It is solely your responsability.
