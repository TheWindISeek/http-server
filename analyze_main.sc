@main def main() = {
    importCpg("/home/lucifer/Codes/test/http-server/workspace/http-server/cpg.bin.zip")
    
    println("Analyzing main function graphs...")
    
    // 查找main函数
    val mainMethod = cpg.method.name("main").l
    
    // 导出CFG为DOT格式
    println("Generating CFG...")
    val cfgDot = mainMethod.head.dotCfg
    os.write(os.pwd / "main_cfg.dot", cfgDot)
    println("CFG has been exported to main_cfg.dot")
    
    // 导出PDG为DOT格式
    println("Generating PDG...")
    val pdgDot = mainMethod.head.dotPdg
    os.write(os.pwd / "main_pdg.dot", pdgDot)
    println("PDG has been exported to main_pdg.dot")
} 