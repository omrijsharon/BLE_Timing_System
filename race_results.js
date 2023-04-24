function loadTable() {
  setInterval(updateTable, 1000);
}

function updateName(index, newName) {
  fetch(`/update_name?index=${index}&name=${encodeURIComponent(newName)}`, {
    method: "POST",
  }).catch((error) => {
    console.error("Error updating competitor name:", error);
  });
}

function createRow(index) {
  const row = document.createElement("tr");
  row.id = index;

  const nameCell = document.createElement("td");
  const nameInput = document.createElement("input");
  nameInput.type = "text";
  nameInput.id = `${index}_name`;
  nameInput.oninput = () => updateName(index, nameInput.value);
  nameCell.appendChild(nameInput);
  row.appendChild(nameCell);

  const lapCountCell = document.createElement("td");
  lapCountCell.id = `${index}_lapCount`;
  row.appendChild(lapCountCell);

  const lastLapTimeCell = document.createElement("td");
  lastLapTimeCell.id = `${index}_lastLapTime`;
  row.appendChild(lastLapTimeCell);

  const bestLapTimeCell = document.createElement("td");
  bestLapTimeCell.id = `${index}_bestLapTime`;
  row.appendChild(bestLapTimeCell);

  const rssiCell = document.createElement("td");
  rssiCell.id = `${index}_rssi`;
  row.appendChild(rssiCell);

  return row;
}

function updateTable() {
  fetch("/get_results")
    .then((response) => response.json())
    .then((data) => {
      const table = document.querySelector("table");

      for (let i = 0; i < data.length; i++) {
        let row = document.getElementById(`${i + 1}`);

        if (!row) {
          row = createRow(i + 1);
          table.appendChild(row);
        }

        const nameInput = document.getElementById(`${i + 1}_name`);
        if (nameInput.value !== data[i].name) {
          nameInput.value = data[i].name;
        }

        document.getElementById(`${i + 1}_lapCount`).innerText = data[i].lapsCount;
        document.getElementById(`${i + 1}_lastLapTime`).innerText = (data[i].lastLapTime / 1000).toFixed(2);
        document.getElementById(`${i + 1}_bestLapTime`).innerText = (data[i].bestLapTime / 1000).toFixed(2);
        document.getElementById(`${i + 1}_rssi`).innerText = data[i].rssi;
      }
    })
    .catch((error) => {
      console.error("Error fetching race results:", error);
    });
}

function resetContestant(index) {
  fetch(`/reset_contestant?index=${index}`)
    .then((response) => {
      if (response.ok) {
        alert(`Contestant ${index} has been reset.`);
        updateTable();
      } else {
        alert(`Error resetting contestant ${index}.`);
      }
    })
    .catch((error) => {
      console.error("Error resetting contestant:", error);
    });
}

function deleteContestant(index) {
  fetch(`/delete_contestant?index=${index}`)
    .then((response) => {
      if (response.ok) {
        alert(`Contestant ${index} has been deleted.`);
        // Remove the row from the table
        const row = document.getElementById(`${index}`);
        row.parentNode.removeChild(row);
        updateTable();
      } else {
        alert(`Error deleting contestant ${index}.`);
      }
    })
    .catch((error) => {
      console.error("Error deleting contestant:", error);
    });
}