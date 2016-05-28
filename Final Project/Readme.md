
# 01219351 - Web Application Development

##Final Group Project

![Image](https://github.com/panpan2557/WebApp6-Berlem.CC/blob/master/Final%20Project/imgs/Pic1.png)


**Load Balance (1)**


> ** HaProxy**

> - หน้าที่: เป็น Load Balancer คอยจัดการ การเข้าถึงส่วนต่างๆของเว็บไซต์
> - วิธีติดตั้ง: Linux สามารถติดตั้ง HaProxy ได้ง่ายๆโดยใช้คำสั่ง apt-get install haproxy แล้ว config ค่าไฟล์ haproxy.cfg ก็สามารถใช้งานได้แล้ว


**Web Server (2)**
> ** nodeJS**

> - หน้าที่: ใช้ nodeJS ติดตั้ง plugin express เพื่อใช้เป็น webserver ในการรัน ReactJS ของหน้าเว็บไซต์
> - วิธีติดตั้ง: ...

**Master Node (1)**

> ** Hadoop + HBase**

> - หน้าที่: เป็นตัวช่วยใน การจัดการ และควบคุมการทำงานของ Hadoop ระหว่าง Slave Node
> - วิธีติดตั้ง: ติดตั้ง hadoop ตาม guideline ทั่วๆไปแบบ single-node ก่อน แล้วจึง config (อะไรต่อวะ เขียนต่อด้วยว่าต้องเซตให้ตัวนี้เป็น master ใน slave)

**Slave Node (2)**

> ** Hadoop**

> - หน้าที่: เป็นตัวประมวนผลของ Hadoop ที่ได้รับคำสั่งมาจาก MasterNode
> - วิธีติดตั้ง: ติดตั้ง hadoop ตาม guideline ทั่วๆไปแบบ single-node ก่อน แล้วจึง config (อะไรต่อวะ เขียนต่อด้วยว่าต้องเซตตัวนี้ให้เป็น slave ใน master)


----------


### Overall Action

- Initialize HTable
--  สร้าง sentence จาก Book ที่มีอยู่ใน HDFS ให้เข้าไปอยู่ใน HTable (HBase NoSQL)
-- โดยการ MapReduce (รายละเอียดด้านล่าง) 
- User Request
-- User รีเควสผ่านหน้าเว็บ browser โดยการส่ง passage เข้ามา
-- Web server ทำการหั่น passage ให้กลายเป็น sentences
-- Web server ทำการ scan ดูใน HTable ว่ามี sentences นั้นๆไหม
-- Web server ตอบกลับ request (มี/ไม่มี sentence/passage นั้นๆ)


----------


### Reasons why using this approach

- การทำให้ Web server สามารถ scan หาข้อมูลใน HTable ผ่าน HBase-client ได้ ทำให้ไม่ต้องมีการ process ข้อมูลหนักๆในทุก request เหมือนกับการทำ search โดยใช้ MapReduce ในทุกๆ request ทำให้เวลาที่ใช้ในการค้นหา น้อยกว่าแบบ MapReduce มาก
- พวกเราคิดว่ายังไง performance ก็ต้องมาก่อนสำหรับ project นี้ จากเหตุผลข้างบน

### MapReduce Application
**Map()**

> - แต่ละ mapper จะได้หนังสือ 1 ไฟล์
> - Input เป็น Object:key, Text:value

> - แต่ละ mapper จะได้หนังสือ 1 ไฟล์
> - นำ Text:value มา preprocess
> -- Text จริงๆในหนังสือ มี format ที่ไม่เป็นระเบียบมากๆ มี spacebar ประหลาดๆที่มากกว่า 1 ตัว หรือพวกบรรทัดที่มันมากกว่า 1 บรรทัด
>-- ตัดพวกนั้นออกโดยใช้ package ของ Java เรียก BreakIterator ซึ่งค่อนข้างดีที่สุดใน บรรดาวิธีการตัดประโยค
>- Preprocess เสร็จก็นำมา map จริงๆ โดยการ write ลง context
>- Output ของเราคือ Text:key, Text:value
>-- Key → sentence ที่เราแตกได้
>-- Value → #ประโยคที่/เล่มที่ [เรียกรวมๆ = Info]
>- สรุปคือ Object:key, Text:key → Text:key, Text:value


**Reduce()**

>- รวม key เดียวกันเข้าด้วยกัน
>- Text:key, Text[] : values → key กับ set ของ value ที่มี key เดียวกัน
>-- กรณีที่มี value หลายตัวคือ เคสที่ sentence มีอยู่หลายตำแหน่งใน storage
>- ทำการ reduce โดย for each: values เขียนลงไปใน HTable
>-- เขียน key(sentence) + Info ลง HTable Sentence = key , Info = sentence no. + page

----------
**Problem**
- Code MapReduce สามารถรันได้ แต่ไม่สามารถ ดูผลลัพธ์ใน HTable จาก Master ได้เนื่องจาก shell ของ Master Node พังหลังจากรันเสร็จ
- ปัญหาหลักๆของเราเกิดจากการใช้ HBase
- Web Server ถูก config และลงตัว nodeJS ไว้เรียบร้อย แต่ยังไม่ได้เขียนส่วนที่ทำการแตกประโยคจาก user input เพราะตัว Master node ดันมีปัญหาซะก่อน
- กว่าจะเริ่มโปรเจคได้ก็ช้ามาก เนื่องจากมองภาพรวมของโปรเจคไม่ออก ว่าต้องใช้ส่วนไหนในการ MapReduce


----------


**Member Participation**

- Punpikorn Rattanawirojkul 5610545722 
-- รับผิดชอบส่วนของ hadoop คิดวิธีการจัดการไฟล์หนังสือ และ การประยุกต์ใช้ MapReduce ของโปรเจค 
-- เตรียม slide ที่ใช้นำเสนอ
- Kittipat Promdirek 5610546699
-- รับผิดชอบส่วนของ hadoop คิดวิธีการจัดการไฟล์หนังสือ และ การประยุกต์ใช้ MapReduce ของโปรเจค 
-- เตรียม slide ที่ใช้นำเสนอ
- Patawat Watakul 5610545714
-- รับผิดชอบส่วนของ hadoop คิดวิธีการจัดการไฟล์หนังสือ และ การประยุกต์ใช้ MapReduce ของโปรเจค 
-- เตรียม slide ที่ใช้นำเสนอ
