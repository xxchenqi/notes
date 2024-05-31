# Idea EasyExcel

## 创建maven项目

## pom文件配置

```xml
<?xml version="1.0" encoding="UTF-8"?>
<project xmlns="http://maven.apache.org/POM/4.0.0"
         xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
         xsi:schemaLocation="http://maven.apache.org/POM/4.0.0 http://maven.apache.org/xsd/maven-4.0.0.xsd">
    <modelVersion>4.0.0</modelVersion>

    <groupId>cq</groupId>
    <artifactId>test</artifactId>
    <version>1.0-SNAPSHOT</version>

    <dependencies>
        <dependency>
            <groupId>com.google.code.gson</groupId>
            <artifactId>gson</artifactId>
            <version>2.9.0</version>
        </dependency>
        
        <dependency>
            <groupId>com.alibaba</groupId>
            <artifactId>easyexcel</artifactId>
            <version>3.1.0</version>

        </dependency>
        <dependency>
            <groupId>org.projectlombok</groupId>
            <artifactId>lombok</artifactId>
            <version>RELEASE</version>
            <scope>compile</scope>
        </dependency>
    </dependencies>
</project>
```



## bean

```java
import com.alibaba.excel.annotation.ExcelIgnore;
import com.alibaba.excel.annotation.ExcelProperty;
import lombok.EqualsAndHashCode;
import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
@EqualsAndHashCode
public class EntriesDTO {

    @ExcelProperty("Name")
    public String Name;
    @ExcelProperty("Address1")
    public String Address1;
    @ExcelProperty("Address2")
    public String Address2;
    @ExcelProperty("Address3")
    public String Address3;
    @ExcelProperty("City")
    public String City;
    //    @ExcelProperty("State")
    @ExcelIgnore
    public String State;
    //    @ExcelProperty("PostalCode")
    @ExcelIgnore
    public String PostalCode;
    @ExcelProperty("State_PostalCode")
    public String State_PostalCode;
    @ExcelProperty("CountryCode")
    public String CountryCode;

    @ExcelProperty("TelCtryCode")
    public String TelCtryCode;
    @ExcelProperty("TelAreaCode")
    public String TelAreaCode;
    @ExcelProperty("Phone")
    public String Phone;
    @ExcelProperty("TelExt")
    public String TelExt;

    @ExcelProperty("FaxCtryCode")
    public String FaxCtryCode;
    @ExcelProperty("FaxAreaCode")
    public String FaxAreaCode;
    @ExcelProperty("Fax")
    public String Fax;
    @ExcelProperty("FaxExt")
    public String FaxExt;

    @ExcelProperty("Url")
    public String Url;
    @ExcelProperty("Brands")
    public String Brands;
    @ExcelProperty("Categories")
    public String Categories;

}
```



```java
import com.alibaba.excel.EasyExcel;
import com.alibaba.excel.write.style.column.LongestMatchColumnWidthStyleStrategy;
import com.google.gson.Gson;
import util.TestFileUtil;

import java.io.*;
import java.util.HashMap;
import java.util.List;

public class Test {
    //    public static String path = "E:\\project\\project_java\\test\\src\\main\\resources\\all.txt";
    public static String path = "E:\\project\\project_java\\test\\src\\main\\resources\\p_t.txt";
    public static HashMap<String, Integer> code = new HashMap<String, Integer>();
    public static HashMap<String, String> nameAll = new HashMap<String, String>();

    public static void main(String[] args) throws Exception {
        initCode();
        initName();


        File file = new File(path);
        String jsonStr = readerMethod(file);
        Gson gson = new Gson();
        CompanyBean companyBean = gson.fromJson(jsonStr, CompanyBean.class);


        List<EntriesDTO> entries = companyBean.entries;
        for (EntriesDTO entry : entries) {
            //增加 State_PostalCode
            entry.State_PostalCode = (entry.State + " " + entry.PostalCode).trim();

            //增加 TelCtryCode
            if (entry.getCountryCode() != null && code.get(entry.getCountryCode()) != null) {
                entry.TelCtryCode = code.get(entry.getCountryCode()).toString().trim();
            }
            //增加 FaxCtryCode
            if (entry.TelCtryCode != null) {
                entry.FaxCtryCode = entry.TelCtryCode.trim();
            }
//            (714) 529-2222
            if (entry.Phone != null && !"".equals(entry.Phone)) {
                String[] split = entry.Phone.split("\\)");
                if (split.length == 2) {
                    //切割Phone
                    entry.Phone = split[1].trim();
                    //增加 TelAreaCode
                    entry.TelAreaCode = split[0].replaceAll("\\(", "").trim();
                }
            }
            if (entry.Fax != null && !"".equals(entry.Fax)) {
                String[] split = entry.Fax.split("\\)");
                if (split.length == 2) {
                    //切割Fax
                    entry.Fax = split[1].trim();
                    //增加 FaxAreaCode
                    entry.FaxAreaCode = split[0].replaceAll("\\(", "").trim();
                }
            }


            //去除电话号码区号
//            529-2222
            if (entry.Phone != null && !"".equals(entry.Phone)) {
                String[] split = entry.Phone.split("-");
                if (split.length == 2) {
                    if (split[0].equals(entry.TelCtryCode)) {
                        entry.Phone = split[1].trim();
                    }
                }
            }


            //去除Fax区号
//            529-2222
            if (entry.Fax != null && !"".equals(entry.Fax)) {
                String[] split = entry.Fax.split("-");
                if (split.length == 2) {
                    if (split[0].equals(entry.FaxCtryCode)) {
                        entry.Fax = split[1].trim();
                    }
                }
            }

            //修改CountryCode
            entry.CountryCode = nameAll.get(entry.CountryCode);
        }

        simpleWrite(companyBean.entries);

//        int A_E = 0;
//        int F_J = 0;
//        int K_O = 0;
//        int P_T = 0;
//        int U_Z = 0;
//
//        for (EntriesDTO entry : entries) {
//            String name = entry.getName();
//            char c1 = name.charAt(0);
//            int c = c1;
//            if ((c >= 97 && c <= 101) || (c >= 65 && c <= 69)) {
//                A_E++;
//            } else if ((c >= 102 && c <= 106) || (c >= 70 && c <= 74)) {
//                F_J++;
//            } else if ((c >= 107 && c <= 111) || (c >= 75 && c <= 79)) {
//                K_O++;
//            } else if ((c >= 112 && c <= 116) || (c >= 80 && c <= 84)) {
//                P_T++;
//            } else if ((c >= 117 && c <= 122) || (c >= 85 && c <= 90)) {
//                U_Z++;
//            }
//        }
//        System.out.println("A_E="+A_E);
//        System.out.println("F_J="+F_J);
//        System.out.println("K_O="+K_O);
//        System.out.println("P_T="+P_T);
//        System.out.println("U_Z="+U_Z);


    }

    public static void initCode() {
        code.put("DNK", 45);
        code.put("CHE", 41);
        code.put("MEX", 52);
        code.put("SCT", 44);
        code.put("CHN", 86);
        code.put("JPN", 81);
        code.put("PRT", 351);
        code.put("GBR", 44);
        code.put("AUS", 61);
        code.put("AUT", 43);
        code.put("USA", 1);
        code.put("ESP", 34);
        code.put("HKG", 852);
        code.put("ISR", 972);
        code.put("TWN", 886);
        code.put("POL", 48);
        code.put("KOR", 82);
        code.put("BRA", 55);
        code.put("TUR", 90);
        code.put("CAN", 1);
        code.put("RUS", 7);
        code.put("FRA", 33);
        code.put("DEU", 49);
        code.put("LVA", 371);
        code.put("ARG", 54);
        code.put("ITA", 39);
        code.put("IND", 91);
    }


    public static void initName() {
        nameAll.put("DNK","Denmark");
        nameAll.put("CHE","Switzerland");
        nameAll.put("MEX","Mexico");
        nameAll.put("SCT","Scotland");
        nameAll.put("CHN","China");
        nameAll.put("JPN","Japan");
        nameAll.put("PRT","Portugal");
        nameAll.put("GBR","Great Britain and Northern Ireland");
        nameAll.put("AUS","Australia");
        nameAll.put("AUT","Austria");
        nameAll.put("USA","United States");
        nameAll.put("ESP","Spain");
        nameAll.put("HKG","Hong Kong");
        nameAll.put("ISR","Israel");
        nameAll.put("TWN","Taiwan");
        nameAll.put("POL","Poland");
        nameAll.put("KOR","Korea, the Republic of");
        nameAll.put("BRA","Brazil");
        nameAll.put("TUR","Turkey");
        nameAll.put("CAN","Canada");
        nameAll.put("RUS","Russia");
        nameAll.put("FRA","France");
        nameAll.put("DEU","Germany");
        nameAll.put("LVA","Latvia");
        nameAll.put("ARG","Argentina");
        nameAll.put("ITA","Italy");
        nameAll.put("IND","India");
    }

    public static void simpleWrite(List<EntriesDTO> data) {
        String fileName = TestFileUtil.getPath() + "simpleWrite" + System.currentTimeMillis() + ".xlsx";
        EasyExcel.write(fileName, EntriesDTO.class)
                .registerWriteHandler(new LongestMatchColumnWidthStyleStrategy())
                .sheet("模板")
                .doWrite(data);
    }

    private static String readerMethod(File file) throws IOException {
        FileReader fileReader = new FileReader(file);
        Reader reader = new InputStreamReader(new FileInputStream(file), "Utf-8");
        int ch = 0;
        StringBuffer sb = new StringBuffer();
        while ((ch = reader.read()) != -1) {
            sb.append((char) ch);
        }
        fileReader.close();
        reader.close();
        String jsonStr = sb.toString();
        return jsonStr;
    }


}

```





```java
import java.io.File;
import java.io.InputStream;

public class TestFileUtil {


    public static InputStream getResourcesFileInputStream(String fileName) {
        return Thread.currentThread().getContextClassLoader().getResourceAsStream("" + fileName);
    }

    public static String getPath() {
        return TestFileUtil.class.getResource("/").getPath();
    }

    public static File createNewFile(String pathName) {
        File file = new File(getPath() + pathName);
        if (file.exists()) {
            file.delete();
        } else {
            if (!file.getParentFile().exists()) {
                file.getParentFile().mkdirs();
            }
        }
        return file;
    }

    public static File readFile(String pathName) {
        return new File(getPath() + pathName);
    }

    public static File readUserHomeFile(String pathName) {
        return new File(System.getProperty("user.home") + File.separator + pathName);
    }
}

```

