async function mainLoop(){
    drawScene();

    requestAnimationFrame(mainLoop);
}

async function main(){
    initGrid();
    await loadScene();
    await resizeCanvas();
    waitForStateChange();
    radioButtons();
    
    mainLoop();
}
main();