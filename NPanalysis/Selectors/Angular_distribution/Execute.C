void Execute(){
	TChain * chain = new TChain("PhysicsTree","PhysicsTree");
	//chain->Add("../Data/old/run256_LNLmerge.root");
	chain->Add("../Data/old/run256_wElThHeavy.root");
	chain->Add("../Data/old/run257_wElThHeavy.root");
	chain->Process("Analyzer.C");
	return;

}
