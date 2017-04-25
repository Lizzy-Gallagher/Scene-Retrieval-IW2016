function intersect(currentData, tempData) {
  return currentData.filter(function(element) {
      return tempData.some(function(e) {
        if (e.scene_hash == element.scene_hash) {
          element.value += e.value
          
          if (typeof element.objects === 'undefined') {
            element.objects = e.objects
          } else if (typeof e.objects !== 'undefined') {
            element.objects = element.objects.concat(e.objects)
          }
        }

        return e.scene_hash == element.scene_hash
      })
  })
}

function difference(currentData, tempData) {
  return currentData.filter(function(element) {
    let exists = tempData.some(function(e) {
      return e.scene_hash == element.scene_hash
    })
    return !exists
  })
}

function sortByValue(arr) {
  return arr.sort(function(a,b) {
    return b.value - a.value
  })
}

export function filter(tempData) {
  var currentData = tempData[0].results;
  for (var i = 1; i < tempData.length; i++) {
    if (tempData[i].toInclude > 0)
      currentData = intersect(currentData, tempData[i].results)
    else {
      currentData = difference(currentData, tempData[i].results)
    }
  }
  return sortByValue(currentData);
}
  