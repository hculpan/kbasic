5 CLEAR:SCNCLR
10 PRINT TAB(32);"Hammurabi"
20 PRINT TAB(15);"Creative Computing Morristown, NJ"
30 PRINT:PRINT:PRINT
80 PRINT "Try your hand at governing ancient Sumerica"
90 PRINT "For a ten-year term of office.":PRINT
95 B1=0:P1=0
100 Z=0:P=95:S=2800:H=3000:E=H-S
110 Y=3:A=H/Y:I=5:Q=1
210 D=0
215 PRINT:PRINT:PRINT "Hammurabi:   I beg to report to you,":Z=Z+1
217 PRINT "In year ";Z;", ";D;" people starved, ";I;" came to the city,"
218 P=P+I
227 IF Q>0 THEN GOTO 230
228 P=INT(P/2)
229 PRINT "A horrible plague struck!  Half the people died!"
230 PRINT "Population is now ";P
232 PRINT "The city now owns ";A;" acres."
235 PRINT "You harvested ";Y;" bushels per acre."
250 PRINT "Rats ate ";E;" bushels."
260 PRINT "You now have ";S;" bushels in store.":PRINT
270 IF Z=11 THEN 860
310 C=INT(10*RND(0)):Y=C+17
312 PRINT "Land is trading at ";Y;" bushels per acre."
320 PRINT "How many acres do you wish to buy";
321 INPUT Q:IF Q<0 THEN 850
322 IF Y*Q<=S THEN 330
323 GOSUB 710
324 GOTO 320
330 IF Q=0 THEN 340
331 A=A+Q:S=S-Y*Q:C=Q
334 GOTO 400
340 PRINT "How many acres do you wish to sell";
341 INPUT Q:IF Q<0 THEN 850
342 IF Q<A THEN 350
343 GOSUB 720
344 GOTO 340
350 A=A-Q:S=S+Y*Q:C=0
400 PRINT
410 PRINT "How many bushels do you wish to feed your people";
411 INPUT Q
412 IF Q<0 THEN 850
418 REM *** TRYING TO USE MORE GRAIN THAN IS IN SILOS?
420 IF Q<=S THEN 430
421 GOSUB 710
422 GOTO 410
430 S=S-Q:C=1:PRINT
440 PRINT "How many acres do you wish to plant with seed";
441 INPUT D: IF D=0 THEN 511
442 IF D<= 0 THEN 850
444 REM *** TRYING TO PLANT MORE ACRES THAN YOU OWN?
445 IF D<=A THEN 450
446 GOSUB 720
447 GOTO 440
449 REM *** ENOUGH GRAIN FOR SEED?
450 IF INT(D/2)<=S THEN 455
452 GOSUB 710
453 GOTO 440
454 REM *** ENOUGH PEOPLE TO TEND THE CROPS?
455 IF D<=10*P THEN 510
460 PRINT "But you have only ";P;" people to tend the fields!  Now then,"
470 GOTO 440
510 S=S-INT(D/2)
511 GOSUB 800
512 REM *** A BOUNTIFUL HARVEST
515 Y=C:H=D*Y:E=0
521 GOSUB 800
522 IF INT(C/2)<>C/2 THEN 530
523 REM *** RATS ARE RUNNING WILD!
525 E=INT(S/C)
530 S=(S-E)+H
531 GOSUB 800
532 REM *** LET'S HAVE SOME BABIES
535 I=INT(C*((20*A-S)/P)/100+1)
539 REM *** HOW MANY PEOPLE HAD FULL TUMMIES?
540 C=INT(Q/20)
541 REM *** HORROR, A 15% CHANGE OF PLAGUE
542 Q=INT(10*(2*RND(0)-0.3))
550 IF P<C THEN 210
551 REM *** STARVE ENOUGH FOR IMPEACHMENT
552 D=P-C:IF D>0.45*P THEN 560
553 P1=((Z-1)*P1+D*100/P)/Z
555 P=C:D1=D1+D:GOTO 215
560 PRINT:PRINT "You starved ";D;" people in one year!!!"
565 PRINT "Due to this extreme mismanagement you have not only"
566 PRINT "been impeached and thrown out of office, but you have"
567 PRINT "also been declared national fink!!!":GOTO 990
710 PRINT "Hammurabi:   Think again.  You have only"
711 PRINT S;" bushels of grain.  Now then,"
712 RETURN
720 PRINT "Hammurabi:   Think again.  You own only ";A;" acres.  Now then,"
730 RETURN
800 C=INT(RND(0)*5)+1
801 RETURN
850 PRINT:PRINT "Hammurabi:   I cannot do what you wish."
855 PRINT "Get yourself another steward!!!!!"
857 GOTO 990
860 PRINT "In your 10-year term of office, ";P1;" percent of the"
862 PRINT "population starved per year on the average, i.e., a total of"
865 PRINT D1;" people died!":L=A/P
870 PRINT "You started with 10 acres per person and ended with"
875 PRINT L;" acres per person.":PRINT
880 IF P1>33 THEN 565
890 IF P1>10 THEN 940
892 IF L<9 THEN 940
895 IF P1>3 THEN 960
896 IF L<10 THEN 960
900 PRINT "A fantastic performance!!!  Charlemagne, Disraeli, and"
905 PRINT "Jefferson combined could not have done better!!":GOTO 990
940 PRINT "Your heavy-handed performance smacks of Nero and Ivan IV."
945 PRINT "The people (remaining) find you an unpleasant ruler, and,"
950 PRINT "frankly, hate your guts!!":GOTO 990
960 PRINT "Your performance could have been somewhat better, but"
965 PRINT "really wasn't too bad at all.  ";INT(P*0.8*RND(0));" people"
970 PRINT "dearly like to see you assassinated but we all have our"
975 PRINT "trivial problems."
990 PRINT:FOR N=1 TO 5:PRINT ".":NEXT N
995 PRINT "So long for now.":PRINT
999 END
