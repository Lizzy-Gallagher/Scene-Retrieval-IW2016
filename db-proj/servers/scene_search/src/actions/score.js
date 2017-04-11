function maximum(results) {
    var max = -1;
    for (var i = 0; i < results.length; i++) {
        if (results[i].value > max)
        max = results[i].value
    }
    return max
}

function minimum(results) {
    var min = Infinity;
    for (var i = 0; i < results.length; i++) {
      if (results[i].value < min)
        min = results[i].value
    }
    return min
}

export function score(tempData) {
    var max = maximum(tempData)
    var min = minimum(tempData)
    tempData = tempData.map(function(e){
      e.value = 100 * ((e.value - min) / (max - min))
      return e
    })
    return tempData
}