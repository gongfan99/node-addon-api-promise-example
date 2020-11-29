var addon = require('bindings')('addon');

(async () => {
  try {
    console.log("2 x 3.3 =", await addon.multiply(2, 3.3));
    console.log("2.01 x 3.3 =", await addon.multiply(2.01, 3.3));
    console.log("2 x 3.3 =", await addon.multiply(2));
  } catch(err){
    console.log("Error:", err.message);
  }
})()