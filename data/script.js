if (!!window.EventSource) {
  let source = new EventSource('/events');
  let block = document.querySelector('.block');
  
  // create event listener for Event source
  source.addEventListener('new_readings', function(e) {
    console.log("new_readings", e.data);
  //parse Json string data
    let myObj = JSON.parse(e.data);
    console.log(myObj);
    let valueLeft = myObj.yLeft;    
    let valueRight = myObj.yRight;
    let valueDown = myObj.xDown;
    let valueUp = myObj.xUp;
    let goRight = block.offsetLeft;
    let goLeft = block.offsetLeft;
    let goDown = block.offsetTop;
    let goUp = block.offsetTop;
// method passes object coordinates
    let blockCoord = block.getBoundingClientRect();
    
    if ((valueLeft > 0) && (blockCoord.left > 0)){
    block.style.left = goLeft - (valueLeft / 100) +"px";
    }
    if ((valueRight > 0) && (blockCoord.left < (window.innerWidth - 200))){
    block.style.left = goRight + (valueRight / 100) +"px";
    }
    if ((valueDown > 0) && (blockCoord.top < (window.innerHeight - 100))){
      block.style.top = goDown + (valueDown / 100) +"px";
    }
    if ((valueUp > 0) && (blockCoord.top > 0) ){
      block.style.top = goUp - (valueUp / 100) +"px";
    }
  });
}
