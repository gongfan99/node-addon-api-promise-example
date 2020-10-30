var addon = require('bindings')('addon');

(async () => {
  try {
    console.log("2 x 3.3 =", await addon.multiply(2, 3.3));
  } catch(err){
    console.log(err.message);
  }
})()