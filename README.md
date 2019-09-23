# FlexiCom
### Flexible Communications

This software was developed in 2012 and aimed to offer a flexible tool for researchers to implement radio layouts with a nice GUI and with minimum effort.
Unfortunately, *this software was discontinued*.

I uploaded the last version we had in order to release it publicly. 

**THIS SOFTWARE IS PRESENTED AS IS. THERE IS NO GUARANTEE THAT STILL WORKS**

In 2012 Software Defined Radio was a primitive topic and GNU Radio was the top of SDR. However, in this epoch the things were too rigid to work with them for implementing and prototyping new waveforms and radio communications.

We developed FlexiCom to suit the first demonstrator of Visible Light Communication (VLC, aka Li-Fi) in 2012. It based on the standard IEEE 802.5.7. It was the first demonstrator run this standard and it worked well.

We publiced in IEEE Communications Magazine, in 2014. More info at: [IEEE Communications Magazine](https://ieeexplore.ieee.org/document/6852087)

Lasts commits are intended to upgrade the code to work with GNU Radio 3.8 and Qt 5, but no enhancements are added nor planned.

The software runs in Windows 10, Linux and probably in macOS. But as it is unmantained, there is no guarantee that it works on these platforms.

You can find more information at https://flexicom.redyc.com

## Building
Dependencies:
- Compiler: MSVC 2017 for Windows, g++ for Linux
- [Qt 5](www.qt.io)
- [Qwt 6.1.3+](https://qwt.sf.net)
- [Boost 1.68.0+](www.boost.org)
- [GNU Radio 3.8+](www.gnuradio.org)
- [UHD 3.14+](https://github.com/EttusResearch/uhd)

For windows, edit Makefile and put there the paths of necessary libraries. Then, open the MSVC 2017 command prompt at FlexiCom root installation path and type:
```
nmake 
```
and the Flexicom.exe should be created.

For linux, edit GNUMakefile and put there the paths of necessary libraries. Then, open a terminal and type:
```
make -f GNUMakefile
```

## Documentation
The documentation is scarce. Some files are documented but others not. Thus, assume that neither documentation nor support is provided.

## Support
No support is provided. This is a discontinued and unmantained software. You can use it at your own. 

## Last words
Finally, if you are still considering this old software, you are welcome! Good luck!

If you want to share some comments and/or thoughts, do not hesitate to contact me.
