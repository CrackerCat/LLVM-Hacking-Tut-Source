<http://mayuyu.io/2017/06/02/LLVMHacking-0x1/>

`Binary` is the result example

`Transforms/` contains the modified lib/Transforms/ for your reference

Other files are test case

Headers are not included, but it should be fairly easy to re-construct yourself


```

[LLVMBuild] bin/clang -S -emit-llvm  test.mm Utils.mm
Do not go gentle into that good night
Selector NameQWEQWE Obfuscated To: bduebcdavx9na5am
Selector NameUtils Obfuscated To: cpykog4rnlcu4to
Selector Namebduebcdavx9na5am Obfuscated To: g2ewo3y1ljghmww6
Skipping main
Skipping External Function: objc_msgSend
Renaming Function: _ZN11foocxxclass5printEv
Skipping External Function: NSLog
Renaming Function: +[Utils QWEQWE]

```
