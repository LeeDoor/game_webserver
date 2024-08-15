async function mainLoop(){
    drawScene();

    requestAnimationFrame(mainLoop);
}

async function main(){
    initGrid();
    await loadScene();
    resizeCanvas();
    waitForStateChange();
    radioButtons();
    
    mainLoop();
}
main();