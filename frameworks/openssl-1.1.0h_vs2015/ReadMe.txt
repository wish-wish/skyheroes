基于openssl-1.1.0h的vs可编译openssl项目
方便使用vs查看源码

由于openssl需要先使用perl配置后才能生成对应的完整makefile和其他.h头文件等完整源码
从makefile可以看出lib,exe,dll等需要的源文件和编译参数
所有需要先手动编译成功


手动编译openssl:
strawberryperl   http://strawberryperl.com/
nasm             https://www.nasm.us/pub/nasm/releasebuilds/2.13.03rc6/
openssl          https://www.openssl.org/source/

VS2015 x64 本机工具命令提示符
cd /d  /path/to/opensslsource
perl Configure VC-WIN64A   //配置好openssl的makefile，可以看到.lib需要哪些c文件，与CFLAGS -D宏参数
nmake   //编译
nmake test
nmake install    //安装到默认路径  C:\Program Files\OpenSSL

安装后的目录结构
C:\Program Files\OpenSSL
bin
--c_rehash.pl
--libcrypto-1_1-x64.dll
--libcrypto-1_1-x64.pdb
--libssl-1_1-x64.dll
--libssl-1_1-x64.pdb
--openssl.exe
--openssl.pdb
html
--...
include/openssl
--aes.h
--applink.c
--asn1.h
--asn1t.h
--asn1_mac.h
--async.h
--bio.h
--blowfish.h
--bn.h
--buffer.h
--camellia.h
--cast.h
--cmac.h
--cms.h
--comp.h
--conf.h
--conf_api.h
--crypto.h
--ct.h
--des.h
--dh.h
--dsa.h
--dtls1.h
--ebcdic.h
--ec.h
--ecdh.h
--ecdsa.h
--engine.h
--err.h
--evp.h
--e_os2.h
--hmac.h
--idea.h
--kdf.h
--lhash.h
--md2.h
--md4.h
--md5.h
--mdc2.h
--modes.h
--objects.h
--obj_mac.h
--ocsp.h
--opensslconf.h
--opensslv.h
--ossl_typ.h
--pem.h
--pem2.h
--pkcs12.h
--pkcs7.h
--rand.h
--rc2.h
--rc4.h
--rc5.h
--ripemd.h
--rsa.h
--safestack.h
--seed.h
--sha.h
--srp.h
--srtp.h
--ssl.h
--ssl2.h
--ssl3.h
--stack.h
--symhacks.h
--tls1.h
--ts.h
--txt_db.h
--ui.h
--whrlpool.h
--x509.h
--x509v3.h
--x509_vfy.h
--__DECC_INCLUDE_EPILOGUE.H
--__DECC_INCLUDE_PROLOGUE.H
lib/engines-1_1
--capi.dll
--capi.pdb
--padlock.dll
--padlock.pdb
lib
--libcrypto.lib
--libssl.lib
