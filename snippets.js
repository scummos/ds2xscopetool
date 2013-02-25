// Calculates the distance to the next peak
var direction = (ch4[5]-ch4[0]) > 0;
var didChange = false;
for ( var i=0; i<ch4.length-5; i++ ) {
  var newDirection = (ch4[i+5]-ch4[i]) > 0;
  if ( direction != newDirection ) didChange = true
  if ( newDirection && didChange ) break;
}
console.log(i)
i
//