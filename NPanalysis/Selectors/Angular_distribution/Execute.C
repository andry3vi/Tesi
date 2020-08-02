void Execute(){
	TChain * chain = new TChain("PhysicsTree","PhysicsTree");
	chain->Add("../Data/nptreated/nptreated.root");
	//chain->Add("../Data/npsim/COMM19_eff_sim_VMugast_stripfix_npanalyzed.root");
	//chain->Add("../Data/nptreated/simnptreated.root");
	//chain->Add("../Data/nptreated/simnptreated_12p.root");
	//chain->Add("../Data/nptreated/simnptreated_52p.root");

	//chain->Add("../Data/old/run256_wElThHeavy.root");
	//chain->Add("../Data/old/run257_wElThHeavy.root");
	//chain->Add("/home/andrea/Repository/nptool/Outputs/Analysis/analyzed_tmp.root");
	chain->Process("Analyzer_GCutGate.C");
	return;

}
