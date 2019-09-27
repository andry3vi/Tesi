import os
import argparse

def getparser():

	parser = argparse.ArgumentParser(description='script to perform autocalibration of all crystal segment')

	parser.add_argument('-src',
                       type=str,
		       nargs=1,
		       default='60Co',
                       dest='src',
                       help='Calibration Source (22Na 40K 56Co 57Co 60Co 88Y 133Ba 134Cs 137Cs 152Eu 208Pb 226Ra 241Am) [60Co]')

	parser.add_argument('-dwa',
                       type=int,
		       nargs=2,
		       default=[10,5],
                       dest='dwa',
                       help='default fwhm and minmum amplitude for the peaksearch [10 5]')


	parser.add_argument('-crylist',
        	       type=str,
		       nargs=1,
                       required=True,
		       dest='crylist',
                       help='file name of CRYSTAL list')
	
	args = parser.parse_args()

	return parser, args


def getCRYlist (filename):

	file = open(filename, "r")
	
	CRYLIST = []

	for CRY in file:
		if CRY[0] != '#':
			CRYLIST.append(CRY[0:3])

	return CRYLIST			
	


def main():

	parser,args = getparser()

	CRY = getCRYlist(args.crylist[0])
	
	print('--------------------------------------------')
	print('------Auto recalibrator for AGATA CORE------')
	print('--------------------------------------------',end='\n\n')
	   
	    
	MAIN_DIR = os.getcwd()
	
	OUT_LIST = []

	for CRYID in CRY:
		DataDir  = MAIN_DIR + '/Data/' + CRYID
		OutDir  = MAIN_DIR + '/Out/' + CRYID
		ConfDir = MAIN_DIR + '/Conf/' + CRYID
			
		
		if os.path.isdir(OutDir):
	
			os.chdir(ConfDir)
                                  
			cmd_1 = 'cp Trapping.cal Trapping_old.cal'
			cmd_2 = 'for i in {0..35}; do echo -e \"$i\\t1.\\t1.\\t999999.9\\t999999.9\\t1.\\t1.\" >> Trapping.cal; done'
			cmd_3 = 'SortPsaHits -f '+DataDir+'/Psa__0-16-F__Hits.fdat -best 1300 1350 -bpar 1 10000 0 |tee sort_hit.log'
			cmd_4 = 'tail -n 39 sort_hit.log |head -n 36 > sort_hit_nohead.log;'
			cmd_5 = 'awk \'FNR==NR{a[NR]=$4;next}{$4=a[FNR]}1\'  sort_hit_nohead.log Trapping.cal |awk \'{printf \"%2s %10s %10s %10s %10s %10s %10s \\n\", $1,$2,$3,$4,$5,$6,$7}\' > Trapping_tmp.cal;'
			cmd_6 = 'awk \'FNR==NR{a[NR]=$5;next}{$5=a[FNR]}1\'  sort_hit_nohead.log Trapping_tmp.cal |awk \'{printf \"%2s %10s %10s %10s %10s %10s %10s \\n\", $1,$2,$3,$4,$5,$6,$7}\' |tee Trapping.cal;'
		
			os.system(cmd_1)
			os.system(cmd_3)
			os.system(cmd_4)
			os.system(cmd_5)
			os.system(cmd_6)
			#print(cmd)
			
			print('\n\n')
			

	print('\n\n\n')
	for out in OUT_LIST:
		
		print(out)


				
if __name__ == '__main__':
    main()
