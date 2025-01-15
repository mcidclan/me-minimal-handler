## Me Minimal Handling
This is a minimal Me reset exception handler. The Me CPU jumps to the default reset exception vector at 0xBCF00000 after a hardware reset. From here, the status register is cleared, and the EPC register takes the destination address to which we will jump using ERET. The Me program will then continue to run there.

# Requirements
Run `./build.sh` from a bash shell.

## Special Thanks To
- Contributors to psdevwiki.
- The PSP homebrew community, for being an invaluable source of knowledge.
- *crazyc* from ps2dev.org, for pioneering discoveries related to the Media Engine.
- All developers and contributors who have supported and continue to support the scene.

# resources:
- [psp wiki on PSDevWiki](https://www.psdevwiki.com/psp/)
- [uofw on GitHub](https://github.com/uofw/uofw)
- [pspdev on GitHub](https://github.com/pspdev)

*m-c/d*
