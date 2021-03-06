
# 01219351 - Web Application Development

##Final Group Project

![Image](https://github.com/panpan2557/WebApp6-Berlem.CC/blob/master/Final%20Project/imgs/Pic1.png)


**Load Balance (1)**


> **HaProxy**

> - หน้าที่: เป็น Load Balancer คอยจัดการ การเข้าถึงส่วนต่างๆของเว็บไซต์
> - วิธีติดตั้ง: Linux สามารถติดตั้ง HaProxy ได้ง่ายๆโดยใช้คำสั่ง apt-get install haproxy แล้ว config ค่าไฟล์ haproxy.cfg ก็สามารถใช้งานได้แล้ว


**Web Server (2)**

> **NodeJS**

> - หน้าที่: ใช้ nodeJS ติดตั้ง plugin express เพื่อใช้เป็น webserver ในการรัน ReactJS ของหน้าเว็บไซต์
> - วิธีติดตั้ง: 
>	1. Curl nodejs version 6 จาก https://deb.nodesource.com/setup_6.x 
> 	2. ติดตั้งด้วยคำสั่ง sudo apt-get install -y nodejs

**Master Node (1)**

> **Hadoop + HBase + zookeeper**

> - หน้าที่: เป็นตัวช่วยใน การจัดการ และควบคุมการทำงานของ Hadoop ระหว่าง Slave Node
> - วิธีติดตั้ง: ติดตั้ง hadoop ตาม guideline ทั่วๆไปแบบ single-node ก่อน แล้วจึง config 
> 	1. ssh access: เพิ่ม public key ให้ master สามารถ remote เข้า slave ได้ทุกตัว
>	2. สร้างไฟล์ master และ slave โดยข้างในคือ ที่อยู่ตาม hosts ของเครื่อง master และ slave
> 	3. ตั้งค่าไฟล์ hdfs-site.xml, core-site.xml และ mapred-site.xml ให้เป็นชื่อของเครื่อง master
>	4. Restart hadoop & yarn


**Slave Node (2)**

> **Hadoop**

> - หน้าที่: เป็นตัวประมวนผลของ Hadoop ที่ได้รับคำสั่งมาจาก MasterNode
> - วิธีติดตั้ง: ติดตั้ง hadoop ตาม guideline ทั่วๆไปแบบ single-node ก่อน แล้วจึง config 
>	1. ตั้งค่าไฟล์ hdfs-site.xml, core-site.xml และ mapred-site.xml ให้เป็นชื่อของเครื่อง master
>	2. Restart hadoop & yarn

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

![Image](https://github.com/panpan2557/WebApp6-Berlem.CC/blob/master/Final%20Project/imgs/Pic2.png)
----------
**Completion**

![Image](https://github.com/panpan2557/WebApp6-Berlem.CC/blob/master/Final%20Project/imgs/Pic3.png)

----------
**Problem**
- Code MapReduce สามารถรันได้ แต่ไม่สามารถ ดูผลลัพธ์ใน HTable จาก Master ได้เนื่องจาก shell ของ Master Node พังหลังจากรันเสร็จ
- ปัญหาหลักๆของเราเกิดจากการใช้ HBase
	- HBase ใช้แรมสูงมาก (แก้ได้แล้วโดยสร้าง swap)
	- ไม่สามารถ config zookeeper ให้ทำงานร่วมกับ HBase ได้อย่างถูกต้อง
	- สุดท้าย HBase พัง ไม่สามารถเข้าใช้งานแม้กระทั้งห shell ได้เลย
- Web Server ถูก config และลงตัว nodeJS ไว้เรียบร้อย แต่ยังไม่ได้เขียนส่วนที่ทำการแตกประโยคจาก user input เพราะตัว Master node ดันมีปัญหาซะก่อน
- กว่าจะเริ่มโปรเจคได้ก็ช้ามาก เนื่องจากมองภาพรวมของโปรเจคไม่ออก ว่าต้องใช้ส่วนไหนในการ MapReduce


----------


**Member Participation**

- Punpikorn Rattanawirojkul 5610545722 
	- รับผิดชอบส่วนของ hadoop คิดวิธีการจัดการไฟล์หนังสือ และ การประยุกต์ใช้ MapReduce ของโปรเจค 
	- เตรียม slide ที่ใช้นำเสนอ
- Kittipat Promdirek 5610546699
	- รับผิดชอบส่วนของ hadoop คิดวิธีการจัดการไฟล์หนังสือ และ การประยุกต์ใช้ MapReduce ของโปรเจค 
	- เตรียม slide ที่ใช้นำเสนอ
- Patawat Watakul 5610545714
	- รับผิดชอบส่วนของ hadoop คิดวิธีการจัดการไฟล์หนังสือ และ การประยุกต์ใช้ MapReduce ของโปรเจค 
	- เตรียม slide ที่ใช้นำเสนอ
- Chinnabhorn Soonue 5610546231
	- รับผิดชอบในส่วนการติดตั้งและตั้งค่าการทำงาน Hadoop Master, Hadoop Slave, Load Balancer, HBase และ zookeeper (แต่ติดตั้ง zookeeper ไม่สำเร็จ) 
- Chonnipa Kittisiriprasert 5610546222
	- รับผิดชอบส่วน Frontend 
	- ทำหน้าเว็บไซต์ 
	- ติดตั้ง Web Server ด้วย NodeJS
