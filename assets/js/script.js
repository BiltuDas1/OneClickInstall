const DEFAULT_BASE_URL = (window.location.port !== '') ? ('http://' + window.location.host) : ('https://' + window.location.host)

let API_BASE_URL = DEFAULT_BASE_URL;
let API_ENDPOINTS = {
  fetch: '/v1/apps',
  install: '/v1/createToken',
  download: '/v1/download'
};

let appsData = [];
let selectedApps = new Set();
let currentCategory = 'all';

const appContainer = document.getElementById('appContainer');
const selectedCountSpan = document.getElementById('selectedCount');
const btnCountSpan = document.getElementById('btnCount');
const installBtn = document.getElementById('installBtn');
const searchInput = document.getElementById('searchInput');

installBtn.onclick = handleInstall;

// async function fetchConfig() {
//     appContainer.innerHTML = `<div style="text-align: center; padding: 40px; font-size: 18px; color: #1d4ed8;">
//                                 <i class="fa-solid fa-cog fa-spin"></i> Loading Configuration...
//                               </div>`;

//     try {
//         const configURL = `${DEFAULT_BASE_URL}/v1/config`;
//         const response = await axios.get(configURL);

//         const config = response.data.api_config;

//         API_BASE_URL = config.baseUrl;
//         API_ENDPOINTS.fetch = config.listApps || config.fetch;
//         API_ENDPOINTS.install = config.getDownloadLinks || config.install;

//         await fetchData();

//     } catch (error) {
//         console.error("Failed to fetch configuration:", error);
//         appContainer.innerHTML = `<div style="text-align: center; padding: 40px; color: red;">
//                                     <i class="fa-solid fa-triangle-exclamation"></i> Error: Backend configuration (${DEFAULT_BASE_URL}/v1/config) not found.Please ensure the server is running
//                                   </div>`;
//     }
// }

async function fetchData() {

  try {

    const fetchURL = `${API_BASE_URL}${API_ENDPOINTS.fetch}`;
    const response = await axios.get(fetchURL);

    if (response.data.status && Array.isArray(response.data.result)) {

      appsData = response.data.result.map(app => ({
        id: app.id,
        name: app.app_name || app.name,
        category: app.category || 'misc',
        desc: app.description || 'No description available',

        icon_id: app.icon_id || app.id,
        color: app.color || '#333333'
      }));

      sidebarCounts();
      renderApps();

    } else {
      throw new Error('Backend returned status: false or invalid result format.');
    }

  } catch (error) {
    console.error("Failed to fetch app data from backend:", error);
    appContainer.innerHTML = `<div style="text-align: center; padding: 40px; color: red;">
                                    <i class="fa-solid fa-triangle-exclamation"></i> Error: Could not fetch data from ${API_BASE_URL}${API_ENDPOINTS.fetch}.
                                  </div>`;
  }
}

async function handleInstall() {
  if (selectedApps.size === 0) {
    alert("Please select at least one application to install.");
    return;
  }

  installBtn.innerHTML = `Downloading... <i class="fa-solid fa-circle-notch fa-spin"></i>`;
  installBtn.disabled = true;

  const installList = Array.from(selectedApps);

  try {
    const installURL = `${API_BASE_URL}${API_ENDPOINTS.install}`;
    console.log(installURL)

    const response = await axios.post(installURL, {
      appIds: installList
    });

    const result = response.data;

    if (result.status) {
      // Download the exe
      const downloadUrl = `${API_BASE_URL}${API_ENDPOINTS.download}/${result.tokenID}`
      await axios.get(downloadUrl, { responseType: 'blob' })
        .then((response) => {
          const url = window.URL.createObjectURL(new Blob([response.data]));
          const link = document.getElementById('downloadlink');
          link.href = url;
          link.click();
          window.URL.revokeObjectURL(url);
        })
        .catch((error) => {
          console.error('Download error:', error);
          alert(`Download failed! ${error.message}`);
        });


      selectedApps.clear();
      Footer();

      if (searchInput.value) {
        searchApps();
      } else {
        renderApps();
      }
    } else {
      alert(`Installation failed! Server error: ${result.message || 'Unknown error'}`);
    }

  } catch (error) {
    console.error("Error sending install request:", error);
    alert(`Network Error: Could not reach backend server.`);
  } finally {
    installBtn.disabled = false;
    Footer();
  }
}

function renderApps() {
  appContainer.innerHTML = '';

  const filteredApps = currentCategory === 'all'
    ? appsData
    : appsData.filter(app => app.category === currentCategory);

  if (filteredApps.length === 0) {
    appContainer.innerHTML = `<div style="text-align: center; padding: 40px; color: #64748b; grid-column: 1 / -1;">
                                    No apps found in the current category/search.
                                  </div>`;
    return;
  }

  filteredApps.forEach(app => {
    const isSelected = selectedApps.has(app.id);
    const card = document.createElement('div');
    card.className = `app-card ${isSelected ? 'selected' : ''}`;
    card.onclick = () => toggleSelection(app.id);

    card.innerHTML = `
            <div class="card-top">
                <div class="app-icon" style="background-color: ${app.color};">
                    <img src="icons/${app.icon_id}.png" alt="${app.name} Icon">
                </div>
                <div class="app-details"><h3>${app.name}</h3><p>${app.desc}</p></div>
            </div>
            <div class="card-bottom">
                <div class="select-text">${isSelected ? '<i class="fa-solid fa-check"></i> Selected (Click to Remove)' : 'Click to select'}</div>
                <button class="add-btn">${isSelected ? 'Added' : '+ Add'}</button>
            </div>
        `;
    appContainer.appendChild(card);
  });
}

function toggleSelection(id) {
  if (selectedApps.has(id)) selectedApps.delete(id);
  else selectedApps.add(id);

  Footer();

  const searchInput = document.getElementById('searchInput');
  if (searchInput.value) {
    searchApps();
  } else {
    renderApps();
  }
}

function filterCategory(category, element) {
  searchInput.value = '';
  currentCategory = category;
  document.querySelectorAll('.sidebar li').forEach(li => li.classList.remove('active'));
  element.classList.add('active');
  renderApps();
}

function searchApps() {
  const query = searchInput.value.toLowerCase();

  const filteredApps = appsData.filter(app => app.name.toLowerCase().includes(query));

  appContainer.innerHTML = '';

  if (filteredApps.length === 0) {
    appContainer.innerHTML = `<div style="text-align: center; padding: 40px; color: #64748b; grid-column: 1 / -1;">
                                    No results found for "${query}".
                                  </div>`;
    return;
  }

  filteredApps.forEach(app => {
    const isSelected = selectedApps.has(app.id);
    const card = document.createElement('div');
    card.className = `app-card ${isSelected ? 'selected' : ''}`;
    card.onclick = () => toggleSelection(app.id);

    card.innerHTML = `
            <div class="card-top">
                <div class="app-icon" style="background-color: ${app.color};">
                    <img src="icons/${app.icon_id}.png" alt="${app.name} Icon">
                </div>
                <div class="app-details"><h3>${app.name}</h3><p>${app.desc}</p></div>
            </div>
            <div class="card-bottom">
                <div class="select-text">${isSelected ? '<i class="fa-solid fa-check"></i> Selected (Click to Remove)' : 'Click to select'}</div>
                <button class="add-btn">${isSelected ? 'Added' : '+ Add'}</button>
            </div>
        `;
    appContainer.appendChild(card);
  });
}

function Footer() {
  const count = selectedApps.size;
  selectedCountSpan.innerText = count;

  installBtn.innerHTML = `INSTALL <i class="fa-solid fa-download"></i>`;

  if (count > 0) {
    installBtn.style.opacity = "1";
    installBtn.style.cursor = "pointer";
  } else {
    installBtn.style.opacity = "0.7";
    // installBtn.style.cursor="default";
  }
}

function sidebarCounts() {
  document.getElementById('count-all').innerText = appsData.length;
  const countCat = (cat) => appsData.filter(a => a.category === cat).length;
  document.getElementById('count-browsers').innerText = countCat('browsers');
  document.getElementById('count-development').innerText = countCat('development');
  document.getElementById('count-multimedia').innerText = countCat('multimedia');
  document.getElementById('count-utilities').innerText = countCat('utilities');
}

function openGuide() {
  document.getElementById('guideModal').style.display = 'flex';
}

function closeGuide() {
  document.getElementById('guideModal').style.display = 'none';
}

window.onclick = function (event) {
  const modal = document.getElementById('guideModal');
  if (event.target == modal) {
    modal.style.display = "none";
  }
}

// fetchConfig();

fetchData();