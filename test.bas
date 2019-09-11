10 cls:gosub 4200                                               
20 print "Done"                                                 
99 end                                                          
4200 getkey d$                                                  
4210 z=z+1:if z=8 then 4220:else if z=20 then 4230              
4220 print "*";:goto 4240                                       
4230 z=0:print " ";:goto 4240                                   
4240 if d$=chr$(13) then return:else goto 4200                  
