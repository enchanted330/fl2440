; IMPORTANT INFO ABOUT GETTING STARTED: Lines that start with a
; semicolon, such as this one, are comments.  They are not executed.

; This script has a special filename and path because it is automatically
; launched when you run the program directly.  Also, any text file whose
; name ends in .ahk is associated with the program, which means that it
; can be launched simply by double-clicking it.  You can have as many .ahk
; files as you want, located in any folder.  You can also run more than
; one ahk file simultaneously and each will get its own tray icon.

; SAMPLE HOTKEYS: Below are two sample hotkeys.  The first is Win+Z and it
; launches a web site in the default browser.  The second is Control+Alt+N
; and it launches a new Notepad window (or activates an existing one).  To
; try out these hotkeys, run AutoHotkey again, which will load this file.

; Add Auto Run in Register, add by guowenxue 2010.08.19
; HKEY_LOCAL_MACHINE-SOFTWARE-MICROSOFT-WINDOWS-CURRENTVERSION-RUN
; Create a New String Value, double hit then input the value: 
;        D:\Program green\AutoHotkey\AutoHotkey.exe
 
;-------------------------------------------------------------------
;Windows System:  Hard Disk map and windows command mapped
;-------------------------------------------------------------------

; CTRL+ALT+H Shutdown the computer
^!h::  
  Run shutdown.exe -s -t 5
return

; CTRL+ALT+R Restart the computer without warnning
^!r::
  Run shutdown.exe -r -f -t 5
return


#i::
Run "C:\Program Files\Internet Explorer\IEXPLORE.EXE"
Return

#t::
Run taskmgr.exe
Return

^d::
Run devmgmt.msc
Return


;------------------------------------------------------------
;    Green Program mapped by "ALT" key
;------------------------------------------------------------
!j::
Run C:\Program Files (x86)\SEGGER\JLinkARM_V410i\JLink.exe
return 

!e::
IfWinExist, ahk_class EVERYTHING
WinActivate
else
Run D:\Program Files\Everything\Everything.exe
return


!i::
Run D:\Program Files (x86)\Source Insight 3\Insight3.Exe
return

!v::
Run D:\Program Files\vmware\vmware.exe
Return

!s::
Run D:\Program Files\VanDyke Software\SecureCRT\SecureCRT.exe
Return

!q::
Run D:\Program Files (x86)\QQ\Bin\QQ.exe
Return

!m::
Run D:\Program green\TTplayer\TTPlayer.exe
Return

!x::
Run D:\Program Files\Youdao\Dict\YodaoDict.exe
Return

!h::
Run D:\Program Green\HyperSnap-DX6.31.01\HprSnap6.exe
Return

!d::
Run D:\Program Files\WinSCP\WinSCP.exe
Return


#c::
Run C:\Users\USER\AppData\Local\Google\Chrome\Application\chrome.exe
Return

!f::
Run C:\Program Files (x86)\Mozilla Firefox\firefox.exe
Return

!t::
Run D:\Program Files (x86)\Thunder Network\Thunder\Program\Thunder.exe
Return


#ifWinActive ahk_class VanDyke Software - SecureCRT
!p::
send password{enter}
return
 
!1::
send tftp 0x33000000 u-boot.bin{enter}
return

!2::
send mkimage -A arm -O linux -T filesystem -C gzip -d nrfs.ext2.gz nrfs.ext2_Magic.gz
send {enter}
return

; For AT91SAM9260
!3::
send set bootdelay 1
send {enter}
send set ipaddr 192.168.1.200
send {enter}
send set serverip 192.168.1.160
send {enter}
send set ethaddr 00:11:22:33:40:51
send {enter}
send set bkr 'tftp 20000000 uImage.gz;nand erase 000a0000 500000;nand write 20000000 000a0000 500000'
send {enter}
send set bfs 'tftp 21100000 ramdisk.gz;nand erase 5a0000 500000;nand write 21100000 5a0000 500000'
send {enter}
send set tb 'tftp 20000000 uImage.gz;tftp 21100000 ramdisk.gz;bootm 20000000'
send {enter}
send set bootargs 'loglevel=7 console=/dev/ttyS0,115200 initrd=0x21100000,5M root=/dev/ram0 rw time'
send {enter}
send set bootcmd_orig 'nand read 20000000 a0000 500000;nand read 21100000 5a0000 500000;bootm 20000000'
send {enter}
send set bootcmd 'nand read 20000000 a0000 0x500000;nand read 21100000 5a0000 500000;bootm 20000000'
send {enter}
send save
send {enter}
return

; For AT91SAM9G20
!4::
send set bootdelay 1
send {enter}
send set ipaddr 192.168.1.200
send {enter}
send set serverip 192.168.1.160
send {enter}
send set ethaddr 00:11:22:33:40:51
send {enter}
send set bkr 'tftp 20000000 uImage.gz-9g20;nand erase 000a0000 500000;nand write 20000000 000a0000 500000'
send {enter}
send set bfs 'tftp 21100000 ramdisk.gz;nand erase 5a0000 500000;nand write 21100000 5a0000 500000'
send {enter}
send set tb 'tftp 20000000 uImage.gz-9g20;tftp 21100000 ramdisk.gz;bootm 20000000'
send {enter}
send set bootargs 'loglevel=7 console=/dev/ttyS0,115200 initrd=0x21100000,5M root=/dev/ram0 rw time'
send {enter}
send set bootcmd_orig 'nand read 20000000 a0000 500000;nand read 21100000 5a0000 500000;bootm 20000000'
send {enter}
send set bootcmd 'nand read 20000000 a0000 0x500000;nand read 21100000 5a0000 500000;bootm 20000000'
send {enter}
send save
send {enter}
return

; ctrl+9, set u-boot enviorment
^9::
send set ethaddr 00:08:3e:26:0a:6b{enter}
send set ipaddr 192.168.1.234{enter}
send set serverip 192.168.1.155{enter}
send set bbt 'nand erase 0 50000;tftp 30008000 u-boot.bin;nand write 30008000 0 40000'{enter}
send set bkr 'tftp 30008000 uImage.gz;nand erase 100000 300000;nand write 30008000 100000 300000'{enter}
send set bfs 'tftp 30800000 ramdisk.gz;nand erase 400000 600000;nand write 30800000 400000 600000'{enter}
send set tb 'tftp 30008000 uImage.gz;tftp 30800000 ramdisk.gz;bootm 30008000'{enter}
send set bootcmd 'nand read 30008000 100000 300000;nand read 30800000 400000 600000;bootm 30008000'{enter}
send set bootargs 'console=ttySAC0,115200 initrd=0x30800000,16M root=/dev/ram0 rw'{enter}
send set bootdelay 1{enter}
send save{enter}

#ifWinactive 

; Redefine only when the active window is a console window    
#IfWinActive ahk_class ConsoleWindowClass   

; Close Command Window with Ctrl+w   
$^w::   
WinGetTitle sTitle   
If (InStr(sTitle, "-")=0) {    
Send EXIT{Enter}   
} else {   
Send ^w   
}   

return    

; Paste in command window   
^V::   
; Spanish menu (Editar->Pegar, I suppose English version is the same, Edit->Paste)   
Send !{Space}ep   
return   


; find in command window   
^F::   
; Spanish menu (Editar->find, I suppose English version is the same, Edit->find)   
Send !{Space}ef   
return   

!1::
send batchisp -device at32uc3a3256 -hardware usb -operation erase f memory flash blankcheck loadbuffer buzzer.elf program verify start reset 0 
return

^1::
send h{enter}
send speed 12000{enter}
send loadbin C:\Users\USER\Downloads\s3c2440_init.bin 0{enter}
send setpc 0{enter}
send g{enter}
sleep 1000
send h{enter}
send loadbin C:\Users\USER\Downloads\u-boot-s3c2440.bin 0x33f80000{enter}
send setpc 0x33f80000{enter}
send g{enter}
return


^7::
send loadbin f:\linux\led.bin 0x33000000{enter}
send setpc 0x33000000{enter}
send g{enter}
return

^8::
send loadbin f:\linux\beep.bin 0x33000000{enter}
send setpc 0x33000000{enter}
send g{enter}
return

^9::
send loadbin f:\linux\key.bin 0x33000000{enter}
send setpc 0x33000000{enter}
send g{enter}
return
#ifWinactive
