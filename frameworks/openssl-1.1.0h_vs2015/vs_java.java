package orm;

import java.util.UUID;

/**
 * 方便生成修改vs工程文件
 */
public class Conn {

    public static void main(String[] args) {

        //for vs2015 vcxproj.filters vcxproj
//        String paths = Conn.getStr().trim().replaceAll("\\.obj",".c").replaceAll("\n","").replaceAll("\t","");
        String paths = Conn.getStr().trim().replaceAll("\\.obj",".c");
//        System.out.println(paths);
        String path[] = paths.split(" |\n");
        //vcxproj
//        for(String one:path)
//        {
//            if(one.length()<1) continue;
//            System.out.println("<ClCompile Include=\""+ one+ "\"/>");
//        }

        //vcxproj.filters
        for(String one:path)
        {
            if(one.length()<1) continue;
            //创建目录
//            String dir = one.substring(0,one.lastIndexOf("\\"));
//            String uuid = UUID.randomUUID().toString();
//            System.out.println("<Filter Include=\"源文件\\"+dir+"\">\n" +
//                    "      <UniqueIdentifier>{"+uuid+"}</UniqueIdentifier>\n" +
//                    "    </Filter>");

            //文件
            String dir = one.substring(0,one.lastIndexOf("\\"));
            System.out.println("<ClCompile Include=\""+one+"\">\n" +
                    "      <Filter>源文件\\"+dir+"</Filter>\n" +
                    "    </ClCompile>");
        }
    }


    public static String getStr()
    {
        return new String(
                "apps\\app_rand.obj\n" +
                        "apps\\apps.obj\n" +
                        "apps\\asn1pars.obj\n" +
                        "apps\\ca.obj\n" +
                        "apps\\ciphers.obj\n" +
                        "apps\\cms.obj\n" +
                        "apps\\crl.obj\n" +
                        "apps\\crl2p7.obj\n" +
                        "apps\\dgst.obj\n" +
                        "apps\\dhparam.obj\n" +
                        "apps\\dsa.obj\n" +
                        "apps\\dsaparam.obj\n" +
                        "apps\\ec.obj\n" +
                        "apps\\ecparam.obj\n" +
                        "apps\\enc.obj\n" +
                        "apps\\engine.obj\n" +
                        "apps\\errstr.obj\n" +
                        "apps\\gendsa.obj\n" +
                        "apps\\genpkey.obj\n" +
                        "apps\\genrsa.obj\n" +
                        "apps\\nseq.obj\n" +
                        "apps\\ocsp.obj\n" +
                        "apps\\openssl.obj\n" +
                        "apps\\opt.obj\n" +
                        "apps\\passwd.obj\n" +
                        "apps\\pkcs12.obj\n" +
                        "apps\\pkcs7.obj\n" +
                        "apps\\pkcs8.obj\n" +
                        "apps\\pkey.obj\n" +
                        "apps\\pkeyparam.obj\n" +
                        "apps\\pkeyutl.obj\n" +
                        "apps\\prime.obj\n" +
                        "apps\\rand.obj\n" +
                        "apps\\rehash.obj\n" +
                        "apps\\req.obj\n" +
                        "apps\\rsa.obj\n" +
                        "apps\\rsautl.obj\n" +
                        "apps\\s_cb.obj\n" +
                        "apps\\s_client.obj\n" +
                        "apps\\s_server.obj\n" +
                        "apps\\s_socket.obj\n" +
                        "apps\\s_time.obj\n" +
                        "apps\\sess_id.obj\n" +
                        "apps\\smime.obj\n" +
                        "apps\\speed.obj\n" +
                        "apps\\spkac.obj\n" +
                        "apps\\srp.obj\n" +
                        "apps\\ts.obj\n" +
                        "apps\\verify.obj\n" +
                        "apps\\version.obj\n" +
                        "apps\\win32_init.obj\n" +
                        "apps\\x509.obj\n" +
                        "ms\\applink.obj"
                );
    }
}
