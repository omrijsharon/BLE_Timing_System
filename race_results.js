function loadTable() {
  setInterval(updateTable, 5000);
}

function updateName(macAddress, newName) {
  fetch(`/update_name?macAddress=${encodeURIComponent(macAddress)}&name=${encodeURIComponent(newName)}`, {
    method: "POST",
  }).catch((error) => {
    console.error("Error updating competitor name:", error);
  });
}

function createRow(macAddress, contestant) {
  const row = document.createElement("tr");
  row.id = macAddress;

  const nameCell = document.createElement("td");
  const nameInput = document.createElement("input");
  nameInput.type = "text";
  nameInput.id = `${macAddress}_name`;
  nameInput.value = contestant.name || contestant.macAddress;
  nameInput.addEventListener("change", () => updateName(macAddress, nameInput.value)); // Add this line
  nameCell.appendChild(nameInput);
  row.appendChild(nameCell);

  const lapCountCell = document.createElement("td");
  lapCountCell.id = `${macAddress}_lapCount`;
  lapCountCell.innerText = contestant.lapsCount;
  row.appendChild(lapCountCell);

  const lastLapTimeCell = document.createElement("td");
  lastLapTimeCell.id = `${macAddress}_lastLapTime`;
  lastLapTimeCell.innerText = (contestant.lastLapTime / 1000).toFixed(2);
  row.appendChild(lastLapTimeCell);

  const bestLapTimeCell = document.createElement("td");
  bestLapTimeCell.id = `${macAddress}_bestLapTime`;
  bestLapTimeCell.innerText = (contestant.bestLapTime / 1000).toFixed(2);
  row.appendChild(bestLapTimeCell);

  const rssiCell = document.createElement("td");
  rssiCell.id = `${macAddress}_rssi`;
  rssiCell.innerText = contestant.rssi;
  row.appendChild(rssiCell);

  const resetCell = document.createElement("td");
  const resetButton = document.createElement("button");
  resetButton.innerText = "Reset";
  resetButton.onclick = () => resetContestant(macAddress);
  resetCell.appendChild(resetButton);
  row.appendChild(resetCell);

  const deleteCell = document.createElement("td");
  const deleteButton = document.createElement("button");
  deleteButton.innerText = "Delete";
  deleteButton.onclick = () => deleteContestant(macAddress);
  deleteCell.appendChild(deleteButton);
  row.appendChild(deleteCell);

  return row;
}

function updateTable() {
  fetch("/get_results")
    .then((response) => response.json())
    .then((data) => {
      const table = document.querySelector("table");
      const existingMacAddresses = new Set();

      for (const contestant of data) {
        const macAddress = contestant.macAddress;
        
        // Skip processing if the macAddress is empty or undefined
        if (!macAddress) {
          continue;
        }

        existingMacAddresses.add(macAddress);
        let row = document.getElementById(macAddress);

        if (!row) {
          row = createRow(macAddress, contestant);
          table.appendChild(row);
        }

        const nameInput = document.getElementById(`${macAddress}_name`);
        if (nameInput.value !== contestant.name && nameInput.value !== contestant.macAddress) {
          nameInput.value = contestant.name || contestant.macAddress;
        }
        document.getElementById(`${macAddress}_lapCount`).innerText = contestant.lapsCount;
        document.getElementById(`${macAddress}_lastLapTime`).innerText = (contestant.lastLapTime / 1000).toFixed(2);
        document.getElementById(`${macAddress}_bestLapTime`).innerText = (contestant.bestLapTime / 1000).toFixed(2);
        document.getElementById(`${macAddress}_rssi`).innerText = contestant.rssi;
      }

      // Remove rows for contestants that are no longer in the data
      // for (const row of table.querySelectorAll("tr")) {
      //   if (!existingMacAddresses.has(row.id)) {
      //     row.remove();
      //   }
      // }
    })
    .catch((error) => {
      console.error("Error fetching race results:", error);
    });
}


function resetContestant(macAddress) {
  fetch(`/reset_contestant?macAddress=${encodeURIComponent(macAddress)}`)
    .then((response) => {
      if (response.ok) {
        alert(`Contestant ${macAddress} has been reset.`);
        updateTable();
      } else {
        alert(`Error resetting contestant ${macAddress}.`);
      }
    })
    .catch((error) => {
      console.error("Error resetting contestant:", error);
    });
}

function deleteContestant(macAddress) {
  fetch(`/delete_contestant?macAddress=${encodeURIComponent(macAddress)}`)
    .then((response) => {
      if (response.ok) {
        alert(`Contestant ${macAddress} has been deleted.`);
        // Remove the row from the table
        const row = document.getElementById(`${macAddress}`);
        row.parentNode.removeChild(row);
        updateTable();
      } else {
        alert(`Error deleting contestant ${macAddress}.`);
      }
    })
    .catch((error) => {
      console.error("Error deleting contestant:", error);
    });
}