document.addEventListener("DOMContentLoaded", () => {
  const resetAllBtn = document.getElementById("resetAllBtn");
  resetAllBtn.addEventListener("click", resetAllContestants);

  // Close the modal when the user clicks the "close" button
  const closeButton = document.querySelector(".close");
  closeButton.addEventListener("click", function () {
    const lapTimesModal = document.getElementById("lapTimesModal");
    lapTimesModal.style.display = "none";
  });

  // Close the modal when the user clicks or taps anywhere on the screen
  window.addEventListener("click", function (event) {
    const lapTimesModal = document.getElementById("lapTimesModal");
    if (lapTimesModal.style.display === "block") {
      lapTimesModal.style.display = "none";
    }
  });

  document.getElementById("algorithm").addEventListener("change", function (event) {
    const selectedAlgorithm = event.target.value;
  
    fetch(`/change_algorithm`, {
      method: "POST",
      headers: {
        "Content-Type": "application/x-www-form-urlencoded",
      },
      body: `algorithm=${encodeURIComponent(selectedAlgorithm)}`,
    })
      .then((response) => {
        if (response.ok) {
          alert(`Algorithm changed to ${selectedAlgorithm}.`);
          // Set the selected attribute for the corresponding option
          const algorithmSelect = document.getElementById("algorithm");
          for (const option of algorithmSelect.options) {
            if (option.value === selectedAlgorithm) {
              option.selected = true;
              break;
            }
          }
        } else {
          alert(`Error changing algorithm.`);
        }
      })
      .catch((error) => {
        console.error("Error changing algorithm:", error);
      });
  });
  // Prevent click event from bubbling up from the content area to the window
  const lapTimesContent = document.getElementById("lapTimesContent");
  lapTimesContent.addEventListener("click", function (event) {
    event.stopPropagation();
  });
});

function loadTable() {
  updateTable();
  setInterval(updateTable, 10000);
}

function updateName(macAddress, newName) {
  fetch(`/update_name?macAddress=${encodeURIComponent(macAddress)}&name=${encodeURIComponent(newName)}`, {
    method: "POST",
  })
    .then((response) => {
      if (response.ok) {
        updateTable();
        console.log("Name updated successfully");
      } else {
        response.text().then((errorText) => {
          console.error("Error updating competitor name:", errorText);
        });
      }
    })
    .catch((error) => {
      console.error("Error updating competitor name:", error);
    });
}

function createRow(macAddress, contestant) {
  const row = document.createElement("tr");
  row.id = macAddress;

  const getLapTimesCell = document.createElement("td");
  const getLapTimesButton = document.createElement("button");
  getLapTimesButton.innerText = "Get Lap Times";
  getLapTimesButton.onclick = () => getLapTimes(macAddress);
  getLapTimesCell.appendChild(getLapTimesButton);
  row.appendChild(getLapTimesCell);

  // Add MAC address column
  const macAddressCell = document.createElement("td");
  macAddressCell.innerText = macAddress;
  row.appendChild(macAddressCell);

  // Add name column with editable input
  const nameCell = document.createElement("td");
  const nameText = document.createElement("span");
  nameText.id = `${macAddress}_name`;
  nameText.innerText = contestant.name || "Enter Name";
  nameText.style.cursor = 'pointer';
  nameText.addEventListener("click", () => {
    const newName = prompt("Enter the new name for the contestant:", "");
    if (newName && newName !== nameText.innerText) {
      updateName(macAddress, newName);
    }
  });
  nameCell.appendChild(nameText);
  row.appendChild(nameCell);

  const lapCountCell = document.createElement("td");
  lapCountCell.id = `${macAddress}_lapCount`;
  lapCountCell.innerText = contestant.lapsCount;
  row.appendChild(lapCountCell);

  const lastLapTimeCell = document.createElement("td");
  lastLapTimeCell.id = `${macAddress}_lastLapTime`;
  lastLapTimeCell.innerText = (contestant.lastLapTime / 1000).toFixed(3);
  row.appendChild(lastLapTimeCell);

  const bestLapTimeCell = document.createElement("td");
  bestLapTimeCell.id = `${macAddress}_bestLapTime`;
  bestLapTimeCell.innerText = (contestant.bestLapTime / 1000).toFixed(3);
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
        if (!macAddress || macAddress.length !== 17) {
          continue;
        }

        existingMacAddresses.add(macAddress);
        let row = document.getElementById(macAddress);

        if (!row) {
          row = createRow(macAddress, contestant);
          table.appendChild(row);
        }

        const nameText = document.getElementById(`${macAddress}_name`);
        if (!nameText.innerText && (contestant.name || contestant.macAddress)) {
          nameText.innerText = contestant.name || "Enter Name";
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
  fetch(`/reset_contestant?macAddress=${encodeURIComponent(macAddress)}`, {
    method: "POST",
  })
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

function resetAllContestants() {
  const shouldResetAll = confirm("Are you sure you want to reset all results?");
  
  if (!shouldResetAll) {
    return;
  }

  fetch(`/reset_all_contestants`, {
    method: "POST",
  })
    .then((response) => {
      if (response.ok) {
        alert(`All contestants have been reset.`);
        updateTable();
      } else {
        alert(`Error resetting all contestants.`);
      }
    })
    .catch((error) => {
      console.error("Error resetting all contestants:", error);
    });
}

function deleteContestant(macAddress) {
  fetch(`/delete_contestant?macAddress=${encodeURIComponent(macAddress)}`, {
    method: "POST",
  })
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

function getLapTimes(macAddress) {
  fetch(`/get_lap_times`, {
    method: "POST",
    headers: {
      "Content-Type": "application/x-www-form-urlencoded",
    },
    body: `macAddress=${encodeURIComponent(macAddress)}`,
  })
    .then((response) => {
      if (response.ok) {
        return response.json();
      } else {
        throw new Error("Error getting lap times.");
      }
    })
    .then((data) => {
      const lapTimes = data.lapTimes;
      let lapTimesString = "";
      lapTimes.forEach((lapTime, index) => {
        lapTimesString += `Lap ${index + 1}: ${(lapTime / 1000).toFixed(2)} seconds\n`;
      });

      // Display lap times in the modal
      const lapTimesContent = document.getElementById("lapTimesContent");
      lapTimesContent.textContent = lapTimesString;
      const lapTimesModal = document.getElementById("lapTimesModal");
      lapTimesModal.style.display = "block";
    })
    .catch((error) => {
      console.error("Error getting lap times:", error);
    });
}

document.getElementById("algorithm").addEventListener("change", function (event) {
  const selectedAlgorithm = event.target.value;

  fetch(`/change_algorithm`, {
    method: "POST",
    headers: {
      "Content-Type": "application/x-www-form-urlencoded",
    },
    body: `algorithm=${encodeURIComponent(selectedAlgorithm)}`,
  })
    .then((response) => {
      if (response.ok) {
        alert(`Algorithm changed to ${selectedAlgorithm}.`);
      } else {
        alert(`Error changing algorithm.`);
      }
    })
    .catch((error) => {
      console.error("Error changing algorithm:", error);
    });
});