import os
import argparse

def getparser():

	parser = argparse.ArgumentParser(description='script to perform autocalibration of all crystal segment')

	parser.add_argument('-source',
                       type=str,
		       default='60Co',
                       dest='src',
                       help='Calibration Source (-22Na -40K -56Co -57Co -60Co -88Y -133Ba -134Cs -137Cs -152Eu -208Pb -226Ra -241Am) [60Co]')

	parser.add_argument('-dwa',
                       type=int,
		       nargs='*',
		       default=[10,5],
                       dest='dwa',
                       help='default fwhm and minmum amplitude for the peaksearch [10 5]')

	args = parser.parse_args()

	return parser, args

def main():

	parser,args = getparser()

	CRY = ["00A", "00B", "00C", "01A", "01C", "02B", "02C", "03A", "03B", "03C", "04A", "04B", "04C", "05A", "05B", "05C", "06A", "06B", "07A", "07B", "07C", "09B", "09C", "10A", "10B", "10C", "11A", "11B", "11C", "12A", "12B", "12C", "13A", "13B", "13C", "14A", "14B", "14C"]
	
	print('--------------------------------------------')
	print('------Auto recalibrator for AGATA data------')
	print('--------------------------------------------',end='\n\n')
	   
	    
	MAIN_DIR = os.getcwd()
	

	for CRYID in CRY:
		OutDir  = MAIN_DIR + '/Out/' + CRYID
		ConfDir = MAIN_DIR + '/Conf/' + CRYID
		print(OutDir)	
		
		if os.path.isdir(OutDir):
	
			os.chdir(ConfDir)
			
			cmd = 'RecalEnergy -spe ' + OutDir + '/Post__5-40-16384-UI__Ener.spec -sub 40 -num 36 -gain 4 -poly1 -lim 4500 5800 -' + args.src + ' -dwa ' + str(args.dwa[0]) +' '+ str(args.dwa[1])+'| tee recal.log'
			os.system(cmd)
			#print(cmd)

						
			os.system('echo \"'+CRYID+'\" >> ../../recalLOG.log')
			os.system('cat recal.log >> ../../recalLOG.log')
			#print(cmd)
			
			print('\n\n')
		        
			extractcmd = 'tail -n 36 recal.log |tee recal_nohead.log ; awk -F\' \' \'{printf \"segm %2.2s %6.3f %.6f \\n\",$1,$16,$17}\' recal_nohead.log | tee RecalEnergy2.cal'
			os.system('cp RecalEnergy2.cal RecalEnergy2_old.cal')	
			
			os.system(extractcmd)
			#print(extractcmd)

			print('\n\n')



				
if __name__ == '__main__':
    main()
