1 rem software by linville, tanstaafl & co.
2 rem program by scott l. richmond
3 rem october 17, 1980 copyright (c) cload 1981
4 gosub 4000:print "how many players (2/6)? ";:a1$="#,###,###":a2$="$#,#######.##":a3$="$,####.##":a4$="$###,#######.##":a5$="#$###":a9$(0)="not ":a9$(1)=""
5 getkey d$:if d$="" then 5: else if d$<"2" or d$>"6" then 5: else print d$:h=val(d$)-1:dim a$(h+1),b$(11),c$(11),d$(7),e(h,6),f(11,h+9),h(11,1),j(h+1):for  i=8 to 11:read b$(i):f(i,h+1)=1000:f(i,h+6)=9000:f(i,h+7)=h+1:f(i,h+8)=int(rnd(0)*24):f(i,h+9)=375000:h(i,0)=375:h(i,1)=350+int(rnd(0)*100)+1
6 next:for  i=0 to 7:read b$(i):f(i,h+1)=2500:f(i,h+6)=97500:f(i,h+7)=h+1:f(i,h+8)=int(rnd(0)*24):next:print :for  i=0 to h:print "name of player #";i+1;:inputa$(i):e(i,0)=1000:e(i,1)=25000:e(i,2)=500:next:a$(h+1)="j.q. smith":h(8,0)=1000:h(8,1)=950+int(rnd(0)*200)+1:for  i=0 to 7
7 h(i,0)=int(rnd(0)*50)+11:h2(i,1)=int(rnd(0)*20)+1+h(i,0):d$(i)="":next:f(8,h+9)=1000000:for  i=0 to 11:read c$(i):next:h1=-1:h7=0:for  i=0 to h:o(i)=i:next
8 h1=h1+1:for  i=0 to 7:h(i,0)=h(i,0)+int(rnd(0)*10)-4:if h(i,0)<10 then h(i,0)=10
9 h(i,1)=h(i,1)+int(rnd(0)*3)-1:if h(i,1)<h(i,0)-5 then h(i,1)=h(i,0)-5
10 d$(i)=d$(i)+chr$(h(i,0)):next:for  o=0 to h:i=o(o):gosub 5000:k(0)=int(rnd(0)*4):k(1)=int(rnd(0)*4)+4:k(2)=int(rnd(0)*4)+8:gosub 4100:for  l=0 to 2:m=k(l):j=int(e(i,0)/h(m,0)):if h(m,1)<h(m,0) then h5=h+1: else h5=h+6
11 k=0:if j>f(m,h5) then j=f(m,h5)
12 if j>0 then print "you may buy";j;"share(s) of ";b$(m);" s to ck.":input"how many shares do you wish  to  buy";k:if k>j or k<0 or k<>int(k) then 12: else f(m,i)=f(m,i)+k:f(m,h5)=f(m,h5)-k:e(i,0)=e(i,0)-k*h(m,0):gosub 4100:if m>7andh5<>h+1 then f(m,h+9)=f(m,h+9)+k*h(m,0)
13 next:gosub 4200:h2=int(rnd(0)*4)+8:h3=8:gosub 3800
14 gosub 4000:print "if  you wish  to  sell some of your s to ck:":print :if f(h2,h+9)<0 then h4=0: else h4=1
15 if f(h3,h+9)<0 then h5=0: else h5=1
16 print b$(h2);" will ";a9$(h4);"buy any c or p or ate s to ck.":print b$(h3);" will ";a9$(h5);"buy any holding company s to ck.":print 
17 print "would you like  to  see your net w or th (y/n)?":gosub 4300:if a$="n" then 20: else gosub 4000:print "statement of net w or th for   ";a$(i):print :print "asset";tab(25);"value":print :print "checking account";tab(25);:print using a4$;e(i,0)
18 print "savings account";tab(25);:print using a4$;e(i,1):print "  net liquid assets";tab(25);:print using a4$;e(i,0)+e(i,1):print :e1=0:for  j=0 to 7:e1=e1+f(j,i)*h(j,0):next:print "c or p or ate s to ck";tab(25);:print using a4$;e1:e2=0:for  j=8 to 11:e2=e2+f(j,i)*h(j,0):next
19 print "holding company s to ck";tab(25);:print using a4$;e2:print "  net s to ck assets";tab(25);:print using a4$;e1+e2:print :print "  net w or th";tab(25);:print using a4$;e1+e2+e(i,0)+e(i,1):gosub 4200:goto 14
20 if h4=0 then 24: else a$="":print "do you wish  to  sell any c or p or ate s to ck (y/n)?":gosub 4300:if a$="n" then 24
21 gosub 4000:gosub 3900:for  j=0 to 7:print j+1;"  ";b$(j);tab(27);h(j,0);tab(32);f(j,i):next:print :print "which s to ck #?":getkey a$:if a$=chr$(13) then 14:if a$<"1" then 21:a$>"8" then 21: else k=val(a$)-1:l=int(f(h2,h+9)/h(k,0)):if l>f(k,i) then l=f(k,i)
22 if l=0 then 14: else print ;"you may sell";l;"share(s) for   $";h(k,0);"@."
23 m=0:input"how many shares do you wish  to  sell";m:if m<0 or m>l or m<>int(m) then 23: else f(k,i)=f(k,i)-m:f(k,h2+h-6)=f(k,h2+h-6)+m:e(i,6)=e(i,6)+m*h(k,0):e(i,0)=e(i,0)+m*h(k,0):f(h2,h+9)=f(h2,h+9)-m*h(k,0):goto 14
24 if h5=0 then 28: else print "do you wish  to  sell any holding company s to ck (y/n)?":gosub 4300:if a$="n" then 28: else gosub 4000:gosub 3900:for  j=8 to 11:print j-7;"  ";b$(j),:print using a5$;h(j,0),:print ,f(j,i):next:print :print "which s to ck #?"
25 getkey a$:if a$="" then 25: else if a$=chr$(13) then 14: else if a$<"1" then  25: else if a$>"4" then 25: else k=val(a)+7:l=int(f(8,h+9)/h(k,0)):if l>f(k,i) then l=f(k,i)
26 print "you may sell";l;"shares for   $";h(k,0);"@."
27 m=0:input"how many shares do you wish  to  sell";m:if m<0 or m>l or m<>int(m) then 27: else f(k,i)=f(k,i)-m:f(k,h+2)=f(k,h+2)+m:e(i,6)=e(i,6)+m*h(k,0):e(i,0)=e(i,0)+m*h(k,0):f(8,h+9)=f(8,h+9)-m*h(k,0):goto 14
28 next:for  i=0 to 11:if f(i,h+8)<>h1 then 36: else gosub 4000:print "this is the s to ckholders' meeting for   ";b$(i):print :print "candidate";tab(26);"# shares":print :for  j=0 to h+1:j(j)=f(i,j):next:for  j=8 to 11:j(f(j,h+7))=j(f(j,h+7))+f(i,h+j-6):next:for  j=0 to h+1
29 if j(j)>0print a$(j);tab(25);:print using a1$;j(j)
30 next:print :print "the current chairman is ";a$(f(i,h+7));".":k=0:l=0:m=0:for  j=0 to h+1:if j(j)>k then k=j(j):l=j:m=0: else if j(j)=k then m=1
31 next:k=f(i,h+7):if m=1print "the chairman has been re-elected as a compromise candidate.":goto 35: else if l=kprint "the chairman has been re-elected.":goto 35: else print a$(l);" has been elected as the new chairman.":f(i,h+7)=l
32 if k<h+1 then if i>7 then e(k,2)=e(k,2)-1750:e(k,0)=e(k,0)+25000:e(k,4)=e(k,4)+25000:f(i,h+9)=f(i,h+9)-25000: else e(k,2)=e(k,2)-1000:e(k,0)=e(k,0)+15000:e(k,4)=e(k,4)+15000
33 if l<h+1 then if i<8 then e(l,2)=e(l,2)+1000: else e(l,2)=e(l,2)+1750
34 if i>7 then f(i,h+9)=f(i,h+9)-25000
35 gosub 4200
36 nextread if f(8,h+2)>0 then f(8,h+6)=f(8,h+6)+f(8,h+2)f(8,h+2)=0
37 if (h1+1)/6<>int((h1+1)/6)goto 8: else for  i=0 to h:o(i)=o(i)+1:if o(i)>h then o(i)=0
40 next:gosub 4000:print "quarterly dividend announcements":print :print "company";tab(27);"$/share";tab(40);"dividend/share":print :for  j=0 to 7:f(j,h+9)=(h(j,0)*5+rnd(100))/100:print b$(j);tab(25);:print using a5$;h(j,0),
41 print tab(40);:print using a3$;f(j,h+9):next:for  i=0 to 7:for  j=0 to h:e(j,0)=e(j,0)+f(i,j)*f(i,h+9):e(j,5)=e(j,5)+f(i,j)*f(i,h+9):next:for  j=8 to 11:f(j,h+9)=f(j,h+9)+f(i,h+j-6)*f(i,h+9):next:if f(i,h+9)<4 then f(i,h+6)=f(i,h+6)+10000
42 next:for  i=8 to 11:f(i,h+9)=f(i,h+9)-10500:f=10000-f(i,h+6):e=f(i,h+9):for  j=0 to 11:e=e+f(j,h+i-6)*h(j,0):next:h(i,1)=int(e/f):next:gosub 4200:gosub 4000:print "holding company s to ck price change announcement":print 
43 print "holding company       old price    new price    outstanding":print tab(48);"shares":print :for  i=8 to 11:print b$(i);tab(23);:print using a5$;h(i,0),:print tab(36);:print using a5$;h(i,1),:print ,10000-f(i,h+6):h(i,0)=h(i,1):h(i,1)=h(i,1)+rnd(19)-10:next
44 gosub 4200:if h1<23 then 8: else for  i=0 to h:gosub 4000:print "yearly income tax for  ";a$(i):print :print "income type";tab(30);"amount";tab(45);"taxable amount":print :print "earned";tab(20);:print using a4$;e(i,4),:e(i,4)=e(i,4)-5000:if e(i,4)<0 then e(i,4)=0
45 print tab(43);:print using a4$;e(i,4):print "interest";tab(20);:print using a4$;e(i,3),:e(i,3)=e(i,3)-400:if e(i,3)<0 then e(i,3)=0
46 print tab(43);:print using a4$;e(i,3):print "dividends";tab(20);:print using a4$;e(i,5),:e(i,5)=e(i,5)/2-500:if e(i,5)<0 then e(i,5)=0
47 print tab(43);:print using a4$;e(i,5):print "s to ck sales";tab(20);:print using a4$;e(i,6),:e(i,6)=e(i,6)/4-25000:if e(i,6)<0 then e(i,6)=0
48 print tab(43);:print using a4$;e(i,6):e=0:for  j=3 to 6:e=e+e(i,j):e(i,j)=0:next:print :print "taxable income";tab(43);:print using a4$;e:e=int(e*25)/100:print :print "income tax";tab(43);:print using a4$;e:e(i,0)=e(i,0)-e:e(i,2)=e(i,2)+400:gosub 4200:next:h1=-1
49 gosub 4000:print "list of corporate chairmanships":print :for  i=0 to h+1:j(i)=0:next:for  i=0 to 7:k=f(i,h+7):print b$(i);tab(35);a$(k):j(k)=j(k)+1:next:gosub 4200:gosub 4000:print "list of holding company chairmanships":print :for  i=8 to 11:k=f(i,h+7):j(k)=j(k)+3
50 print b$(i);tab(35);a$(k):next:print :k=0:for  i=0 to h:if j(i)>10 then print a$(i);" is the winner of the game.":print :end: else next:gosub 4200
51 gosub 4000:print " #";tab(10);"company":print :for  i=0 to 7:print i+1;tab(10);b$(i):next:print @165,"rec or ds have been kept of";:print ,"the c or p or ate s to ck prices";:print @293,"over the last";h7+1;"years.";:print @357,"these rec or ds are shown";
52 print ,"graphically.";:print @832,"which company #?"
53 a=inkey$:if a="" then 53: else if a=chr$(13) then 57: else if a<"1" or a>"8" then 53: else i=val(a)-1:gosub 4000:print "the stock price record of ";b$(i);" for  ";h7+1;"year(s).":print @320,"$140":print @448,"$110":print @576," $80":print @704," $50":print ," $20"
54 print ,"      jfmamjjasond  jfmamjjasond  jfmamjjasond  jfmamjjasond";:l=12:for  j=1 to 24*(1+h7):k=43-int((asc(mid$(d(i),j,1))+1)/5):if k<9 then k=9
55 for m=k to 43:set(l,m):next:l=l+1:if j/24=int(j/24) then l=l+4
56 next:gosub 4200:goto 51
57 h7=h7+1:if h7<4 then 8: else h7=h7-1:for i=0 to 7:d(i)=right$(d(i),72):next:goto 8
1010 data tanstaafl holdings,american holdings,united holdings,evergreen holdings,bellevue square,n or thgate mall,weyhaeuser c or p.,ge or gia-pacif ic,seattle 1st ntl bank,rainier ntl. bank,boeing aerospace,pacif ic car & fndry.
1020 data january,february,march,april,may,june,july,august,september,oc to ber,november,december
3800 for j=0 to 11:if f(j,h+1)=0 then h(j,1)=25000
3810 next:return
3900 print " #   company";tab(27);"$/sh";tab(32);"owned":print :return
4000 scnclr:print "-*- wall street & corporate control -*-":print :return
4100 gosub 4000:print "currently listed on the nyse:":print :gosub 3900:for  n=0 to 2:print n+1"  ";b$(k(n));tab(25);h(k(n),0);tab(32);f(k(n),i):next:print :return
4200 getkey b$
4210 z=z+1:if z=8 then print "*";: else if z=20 then print " ";:z=0
4220 getkey b$:if b$=chr$(13) then return: else goto 4210
4300 a$="":get a$
4310 if a$="y" then return
4320 if a$="n" then return
4330 if a$=chr$(13) then a$="n":return
4340 goto 4300
4395 return: else if a$="y" then a$="y":return: else if a$="n" then a$="n":return: else goto 4310
5000 gosub 4000:print "it is ";a$(i);"'s turn.":print "it is the ";:if h1/2=int(h1/2)  then  print "first";: else print "second";
5010 print " half of ";c$(h1/2);".":print :j=int(e(i,1)/4)/100:e(i,1)=e(i,1)+j:e(i,3)=e(i,3)+j:print "interest income:";tab(25);:print using a2$;j:print "earned income:";tab(25);:print using a2$;e(i,2):e(i,4)=e(i,4)+e(i,2):e(i,0)=e(i,0)+e(i,2)
5020 print :print "checking acct balance:";tab(25);:print using a2$;e(i,0):print "savings acct balance:";tab(25);:print using a2$;e(i,1):print :e(i,0)=int(e(i,0)*100)/100:e(i,1)=int(e(i,1)*100)/100
5030 j=0:input"how much  to  transfer  to  checking";j:j=int(j):if j<0 or j>e(i,1) then 5030: else e(i,0)=e(i,0)+j:e(i,1)=e(i,1)-j:if j>0 then return
5040 j=0:input"how much  to  transfer  to  savings";j:j=int(j):if j=0 then return: else if j<0 or j>e(i,0) then 5040: else e(i,0)=e(i,0)-j:e(i,1)=e(i,1)+j:return
